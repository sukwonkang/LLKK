#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __unix__
#include <unistd.h>
#endif
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cstdlib> // For random number generation
#include <fstream>
#include <sstream>
#include <iostream>
#include <string> // std::string, std::to_string
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <al.h>
#include <alc.h>
#define DR_WAV_IMPLEMENTATION
#include "thirdpr/dr_wav.h"
#include <chrono> // For timing and FPS calculation
#include "configs/config.h"
#include "game.h"
#include "gph.h"


GLFWwindow* window;

Game g = { Player(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2) };
GPH gra;
Camera camera;
// Function to compile shaders
// Shader sources
const char* vertexShaderSource = R"(#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 0.0, 1.0);
    TexCoord = aTexCoord;
})";

const char* fragmentShaderSource = R"(#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;

void main() {
    FragColor = texture(texture1, TexCoord);
})";
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}
void initPlatformRendering(GLuint& VAO, GLuint& VBO, GLuint& EBO)
{
    // Vertex data for the platform (quad) with positions and texture coordinates
    float vertices[] = {
        // Positions          // Texture Coords
        -0.5f, -0.5f,          0.0f, 0.0f,  // Bottom-left
         0.5f, -0.5f,          1.0f, 0.0f,  // Bottom-right
         0.5f,  0.5f,          1.0f, 1.0f,  // Top-right
        -0.5f,  0.5f,          0.0f, 1.0f   // Top-left
    };

    // Index data to define the two triangles for the quad
    unsigned int indices[] = {
        0, 1, 2,   // First triangle
        2, 3, 0    // Second triangle
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind the Vertex Array Object first, then bind and set vertex buffers
    glBindVertexArray(VAO);

    // Bind and set the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind and set the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind the VBO, VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void limitFPS(std::chrono::steady_clock::time_point& lastTime) {
    // Get the current time
    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();

    // Calculate the elapsed time between frames
    std::chrono::duration<double> elapsedTime = currentTime - lastTime;

    // If the elapsed time is less than the target frame time, sleep for the remaining time
    if (elapsedTime.count() < targetFrameTime) {
        // Calculate the remaining time to sleep
        std::chrono::duration<double> sleepDuration(targetFrameTime - elapsedTime.count());

        // Sleep based on the operating system
#ifdef _WIN32
        Sleep(static_cast<DWORD>(sleepDuration.count() * 1000)); // Sleep takes milliseconds on Windows
#elif defined __unix__
        usleep(static_cast<useconds_t>(sleepDuration.count() * 1000000)); // usleep takes microseconds on Unix-like systems
#endif

        // After sleep, update the current time
        currentTime = std::chrono::steady_clock::now();
        elapsedTime = currentTime - lastTime;
    }

    // Update lastTime for the next frame calculation
    lastTime = currentTime;

    // Optional: Print FPS for debugging purposes
    fps = 1.0 / elapsedTime.count();
}

// Error handling function
void checkALError(const std::string& message)
{
    ALenum error = alGetError();
    if (error != AL_NO_ERROR)
    {
        std::cerr << "OpenAL Error (" << message << "): " << alGetString(error) << std::endl;
    }
}

// Function to process player input
void processInput(ALuint source)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        menuVis = true;
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        g.gameover = true;

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    {
        if (g.finalPlatformActive)
        {
            g.reset();
            g.nextLevel();
        }
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        // Reset player position and velocity
        g.player.x = WINDOW_WIDTH / 2;
        g.player.y = WINDOW_HEIGHT / 2;
        g.player.velocityY = 0.0f;
        g.player.velocityX = 0.0f;
        g.player.onGround = false;

        // Reset platforms
        g.platforms.clear(); // Clear existing platforms
        g.initPlatforms(WORLD_WIDTH, WORLD_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);   // Reinitialize platforms
        g.reset();
        g.currentLevel = 1;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (!spacePressed)
        {
            ALint state;
            alGetSourcei(source, AL_SOURCE_STATE, &state);
            //if (state != AL_PLAYING) {
                alGetSourcei(source, AL_SOURCE_STATE, &state);
                alSourcePlay(source);
                checkALError("Playing source");
            //}
                g.player.jump();
            spacePressed = true;
            std::cout << "JUMP." << std::endl;
       
        }
    }
    else
    {
        spacePressed = false; // Reset the flag when space is released
    }

    // Movement direction
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        g.player.velocityX = -150.0f; // Move left
        movingLeft = true;
    }
    else
    {
        movingLeft = false;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        g.player.velocityX = 150.0f; // Move right
        movingRight = true;
    }
    else
    {
        movingRight = false;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        if (!upped)
        {
            camera.setZoom(true);
            upped = true;
        }
    }
    else
    {
        upped = false;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        if (!downed)
        {
            camera.setZoom(false);
            downed = true;
        }
    }
    else
    {
        downed = false;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Adjust the viewport based on the new window dimensions
    glViewport(0, 0, width, height);

    // Update projection matrix to match new window size
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1); // Adjust the projection to the new size
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void showGMUI(float windowWidth, float windowHeight)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Player", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    //  ImGui::PushFont(font2);

    ImGui::Text("Level : %d", g.currentLevel);
    ImGui::Text("Platforme numbers : %d", g.parts.size());
    ImGui::Text("Successful Jumps : %d", g.successfulJumps);
    ImGui::Text("FPS : %d", fps);

    // ImGui::PopFont(); // Restore original font size

    ImGui::End();
    if (menuVis)
    {

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));

        ImGui::Begin("Main Menu", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings);
        ImVec2 windowSize = ImGui::GetWindowSize();
        float centerX = windowSize.x / 2.0f;
        float centerY = windowSize.y / 2.0f;
        // Assuming button width is 200 and height is 50
        float buttonWidth = 200.0f;
        float buttonHeight = 100.0f;
        // Increase font size
        // ImGui::PushFont(font1);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));          // Set text color to yellow
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));        // Set button background color to green
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.8f, 0.8f, 1.0f)); // Darker green on hover
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.3f, 0.0f, 1.0f));

        ImGui::SetCursorPosX(centerX - buttonWidth / 2.0f);
        ImGui::SetCursorPosY(centerY - (buttonHeight * 1.5f));
        if (ImGui::Button("New Game", ImVec2(300, 80)))
        {
            // Reset player position and velocity
            g.player.x = WINDOW_WIDTH / 2;
            g.player.y = WINDOW_HEIGHT / 2;
            g.player.velocityY = 0.0f;
            g.player.velocityX = 0.0f;
            g.player.onGround = false;
            g.platforms.clear();           
            g.initPlatforms(WORLD_WIDTH,WORLD_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
            g.reset();
            g.currentLevel = 1;

            menuVis = false;
        }

        ImGui::Spacing(); // Add some vertical spacing

        ImGui::SetCursorPosX(centerX - buttonWidth / 2.0f);
        ImGui::SetCursorPosY(centerY - buttonHeight / 2.0f);
        if (ImGui::Button("Continue", ImVec2(300, 80)))
        {
            menuVis = false;
            // Open the options menu (you'll need to implement this separately)
        }
        ImGui::Spacing();

        ImGui::SetCursorPosX(centerX - buttonWidth / 2.0f);
        ImGui::SetCursorPosY(centerY + buttonHeight / 2.0f);
        if (ImGui::Button("Options", ImVec2(300, 80)))
        {

            // Quit the game or application
        }
        ImGui::SetCursorPosX(centerX - buttonWidth / 2.0f);
        ImGui::SetCursorPosY(centerY + buttonHeight * 2.0f);
        if (ImGui::Button("Quit", ImVec2(300, 80)))
        {
            glfwSetWindowShouldClose(window, true);

            // Quit the game or application
        }
        // ImGui::PopFont(); // Restore original font size
        ImGui::PopStyleColor(4);

        ImGui::End();
    }
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

ALuint getBufferWaf(const char* path)
{
    drwav wav;
    if (!drwav_init_file(&wav, path, NULL))
    {
        std::cerr << "Error loading WAV file." << std::endl;
        // ... cleanup OpenAL
        return -1;
    }
    drwav_uint64 totalPCMFrameCount = wav.totalPCMFrameCount;
    unsigned int channels = wav.channels;
    unsigned int sampleRate = wav.sampleRate;
    drwav_int16* sampleData = (drwav_int16*)malloc((size_t)totalPCMFrameCount * channels * sizeof(drwav_int16));
    drwav_read_pcm_frames_s16(&wav, totalPCMFrameCount, sampleData);
    drwav_uninit(&wav);

    // Create OpenAL buffer
    ALuint buffer;
    alGenBuffers(1, &buffer);
    checkALError("Generating buffer");

    // Load WAV data into buffer
    ALenum format = channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    alBufferData(buffer, format, sampleData, totalPCMFrameCount * channels * sizeof(drwav_uint16), sampleRate);
    checkALError("Loading sound data into buffer");

    free(sampleData);
    // Create source and play sound
    return buffer;
}

ALuint PlayWaf(ALuint buffer,float volume)
{
    ALuint source;
    alGenSources(1, &source);
    checkALError("Generating source");

    alSourcei(source, AL_BUFFER, buffer);
    checkALError("Attaching buffer to source");
    alSourcef(source, AL_GAIN, volume);
    checkALError("Setting source gain");
    return source;
}

void SetupMGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

// Main function
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Get the primary monitor and its video mode
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
    GLFWmonitor* _primaryMonitor = nullptr;
    if (fullscreen)
    {
        _primaryMonitor = primaryMonitor;
        WINDOW_WIDTH = videoMode->width;
        WINDOW_HEIGHT = videoMode->height;
    }
    // Create window
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GF Game", _primaryMonitor, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Initialize OpenAL
    ALCdevice* device = alcOpenDevice(nullptr);
    if (!device)
    {
        std::cerr << "Failed to open OpenAL device." << std::endl;
        return -1;
    }

    ALCcontext* context = alcCreateContext(device, nullptr);
    if (!context)
    {
        std::cerr << "Failed to create OpenAL context." << std::endl;
        alcCloseDevice(device);
        return -1;
    }

    if (!alcMakeContextCurrent(context))
    {
        std::cerr << "Failed to make context current." << std::endl;
        alcDestroyContext(context);
        alcCloseDevice(device);
        return -1;
    }std::chrono::steady_clock::time_point hhh = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
    ALuint buffer = getBufferWaf("res/s.wav");
    ALuint source = PlayWaf(buffer,0.01f);
    alSourcePlay(source);
    checkALError("Playing source");

    buffer = getBufferWaf("res/gover.wav");
    ALuint source2 = PlayWaf(buffer, 0.1f);

    buffer = getBufferWaf("res/jump.wav");
    ALuint source3 = PlayWaf(buffer,0.1f);

        // Setup Dear ImGui context 
    SetupMGui();

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    // Set up viewport
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glOrtho(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT, -1, 1);

    GLuint textureID = gra.loadTexture("res/img.jpg");
    GLuint textureID2 = gra.loadTexture("res/img2.jpg");

    // Initialize platforms
    g.initPlatforms(WORLD_WIDTH, WORLD_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT);
    g.initParts(WORLD_WIDTH, WORLD_HEIGHT);
    // ImFont* font1 = io.Fonts->AddFontFromFileTTF("C:\\Users\\SS\\Desktop\\GF\\img\\misc\\fonts\\ProtestGuerrilla-Regular.ttf",28.0f);
    // ImFont* font2 = io.Fonts->AddFontFromFileTTF("C:\\Users\\SS\\Desktop\\GF\\img\\misc\\fonts\\ProggyClean.ttf",18.0f);
    //  Main game loop
    //////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<glm::vec2> positions; // Store previous positions
    const int maxTrailLength = 20; // Limit trail length
    Player ffff = Player(100.f, 100.f);
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    //glUseProgram(shaderProgram);

    // Check for linking errors
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking error: " << infoLog << std::endl;
    }
    //GLint modelLoc = glGetUniformLocation(shaderProgram, "model");  // Get location of the "model" uniform in the shader

    while (!glfwWindowShouldClose(window))
    {
        std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
        double deltaTime = std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - hhh).count();
       // lastTime = currentTime;

        // Cap deltaTime to prevent large updates after a freeze
        if (deltaTime > 0.01) // Cap at 100 ms
            deltaTime = 0.01;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        showGMUI(windowWidth, windowHeight);

        // Update the camera based on the player's position
        
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        camera.update(windowWidth, windowHeight,WORLD_WIDTH,WORLD_HEIGHT,g.player.x,g.player.y);
        gra.drCamera(windowWidth, windowHeight, camera.x, camera.y, camera.zoom);

        if (!menuVis)
        {
            gra.drawStripedBorder(WORLD_WIDTH, WORLD_HEIGHT, 5.0f, 200); // For example, 100 stripes and 10.0 thickness
            for (auto& platform : g.platforms)
            {
                gra.renderPlatform(textureID, platform);
            }

            for (auto& part : g.parts)
            {
                gra.renderPart(part.x, part.y, 2.0f, 2.0f);
            }
            
            gra.renderPlayer(textureID, g.player);

            //gra.renderPlayer(textureID,ffff );
            if(!g.gameover)
                g.player.update(deltaTime,WORLD_WIDTH,movingRight,movingLeft);
            //ffff.x = deltaTime * 10000;
            for (auto& platform : g.platforms)
            {
                platform.update(deltaTime,WORLD_HEIGHT,WORLD_WIDTH,g.platforms);
            }

            if (g.finalPlatformActive)
            {
                gra.renderPlatform(textureID, g.finalPlatform);
                g.finalPlatform.update(deltaTime,WORLD_HEIGHT,WORLD_WIDTH,g.platforms);
            }

            // Update position
            positions.push_back({ g.player.x,g.player.y });
            if (positions.size() > maxTrailLength) {
                positions.erase(positions.begin()); // Remove the oldest position
            }
            glColor3f(0.1f, 1.0f, 1.0f);
            // Render the trail
            for (size_t i = 0; i < positions.size(); ++i) {
                gra.renderPart(positions[i].x, positions[i].y-g.player.height/2,1.0f, 1.0f);
                gra.renderPart(positions[i].x+4, positions[i].y - g.player.height / 2, 1.0f, 1.0f);
                gra.renderPart(positions[i].x+8, positions[i].y - g.player.height / 2, 1.0f, 1.0f);
                gra.renderPart(positions[i].x+12, positions[i].y - g.player.height / 2, 1.0f, 1.0f);
                gra.renderPart(positions[i].x-4, positions[i].y - g.player.height / 2, 1.0f, 1.0f);
                gra.renderPart(positions[i].x-8, positions[i].y - g.player.height / 2, 1.0f, 1.0f);
                gra.renderPart(positions[i].x - 12, positions[i].y - g.player.height / 2, 1.0f, 1.0f);

            }
            if (!g.gameover)
                g.checkCollision();
            g.checkGameOver(WORLD_HEIGHT);
            processInput(source3);

         
            if (g.felt)
            {
                ALint state;
                alGetSourcei(source2, AL_SOURCE_STATE, &state);
                if (state != AL_PLAYING) {
                    alGetSourcei(source2, AL_SOURCE_STATE, &state);
                    alSourcePlay(source2);
                    checkALError("Playing source");
                } 
                menuVis = true;
            }
        }

        limitFPS(lastTime);
        hhh = currentTime;
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // Cleanup
    alDeleteSources(1, &source);
    alDeleteSources(1, &source2);
    alDeleteSources(1, &source3);
    alDeleteBuffers(1, &buffer);
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);

    // Cleanup and close
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
