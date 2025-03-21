#include "gph.h"
#include <stdlib.h>
#include <GL/glew.h>
#define STB_IMAGE_IMPLEMENTATION
#include "thirdpr/stb_image.h"
#include <vector>
#include <chrono>
#include "parties/player.h"
#include "particule.h"


GPH::GPH() {}

void GPH::renderPlayer(GLuint textureID,Player& player)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(player.x - player.width / 2, player.y - player.height / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(player.x + player.width / 2, player.y - player.height / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(player.x + player.width / 2, player.y + player.height / 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(player.x - player.width / 2, player.y + player.height / 2);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void GPH::renderPlatform(GLuint textureID, Platform& platform)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(platform.x - platform.width / 2, platform.y - platform.height / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(platform.x + platform.width / 2, platform.y - platform.height / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(platform.x + platform.width / 2, platform.y + platform.height / 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(platform.x - platform.width / 2, platform.y + platform.height / 2);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void GPH::drawStripedBorder(float worldWidth, float worldHeight, float borderThickness, int numStripes)
{
    // Colors for the stripes
    float color1[3] = { 1.0f, 1.0f, 1.0f }; // White
    float color2[3] = { 0.0f, 0.0f, 0.0f }; // Black

    float stripeWidth = worldWidth / numStripes;

    // Draw top and bottom edges with stripes
    for (int i = 0; i < numStripes; ++i)
    {
        float x = i * stripeWidth;

        // Alternate between colors
        if (i % 2 == 0)
        {
            glColor3f(color1[0], color1[1], color1[2]); // Set color to color1
        }
        else
        {
            glColor3f(color2[0], color2[1], color2[2]); // Set color to color2
        }

        // Top border stripe
        glBegin(GL_QUADS);
        glVertex2f(x, worldHeight);                                 // Top-left corner
        glVertex2f(x + stripeWidth, worldHeight);                   // Top-right corner
        glVertex2f(x + stripeWidth, worldHeight - borderThickness); // Bottom-right corner
        glVertex2f(x, worldHeight - borderThickness);               // Bottom-left corner
        glEnd();

        // Bottom border stripe
        glBegin(GL_QUADS);
        glVertex2f(x, 0);                             // Bottom-left corner
        glVertex2f(x + stripeWidth, 0);               // Bottom-right corner
        glVertex2f(x + stripeWidth, borderThickness); // Top-right corner
        glVertex2f(x, borderThickness);               // Top-left corner
        glEnd();
    }

    // Draw left and right edges with stripes
    float stripeHeight = worldHeight / numStripes;
    for (int i = 0; i < numStripes; ++i)
    {
        float y = i * stripeHeight;

        // Alternate between colors
        if (i % 2 == 0)
        {
            glColor3f(color1[0], color1[1], color1[2]); // Set color to color1
        }
        else
        {
            glColor3f(color2[0], color2[1], color2[2]); // Set color to color2
        }

        // Left border stripe
        glBegin(GL_QUADS);
        glVertex2f(0, y);                              // Bottom-left corner
        glVertex2f(borderThickness, y);                // Bottom-right corner
        glVertex2f(borderThickness, y + stripeHeight); // Top-right corner
        glVertex2f(0, y + stripeHeight);               // Top-left corner
        glEnd();

        // Right border stripe
        glBegin(GL_QUADS);
        glVertex2f(worldWidth - borderThickness, y);                // Bottom-left corner
        glVertex2f(worldWidth, y);                                  // Bottom-right corner
        glVertex2f(worldWidth, y + stripeHeight);                   // Top-right corner
        glVertex2f(worldWidth - borderThickness, y + stripeHeight); // Top-left corner
        glEnd();
    }
    glColor3f(color1[0], color1[1], color1[2]); // Set color to color1
}
void GPH::renderPart(float x, float y, float rectWidth, float rectHeight)
{
    // Draw the rectangle
    glBegin(GL_QUADS);
    glVertex2f(x, y);                          // Bottom left
    glVertex2f(x + rectWidth, y);              // Bottom right
    glVertex2f(x + rectWidth, y + rectHeight); // Top right
    glVertex2f(x, y + rectHeight);             // Top left
    glEnd();
}
GLuint GPH::loadTexture(const char* path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Load image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
       // std::cerr << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}
void GPH::drCamera(int windowWidth, int windowHeight,float x,float y,float zoom)
{
    // Adjust the orthographic projection based on zoom
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, windowWidth / zoom, 0, windowHeight / zoom, -1, 1); // Adjust view for zoom
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-x, -y, 0); // Move the camera to follow the player
}


