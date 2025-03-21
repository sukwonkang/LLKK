#include "game.h"
#include <stdlib.h>
#include <vector>
#include <chrono>

Game::Game(Player player) : player( player){}

void Game::update() {
    
}
// Function to check for player-platform collision
void Game::checkCollision()
{
    if (finalPlatformActive)
    {
        // Check if player lands on the final platform
        if (player.x < finalPlatform.x + finalPlatform.width / 2 &&
            player.x > finalPlatform.x - finalPlatform.width / 2 &&
            player.y - player.height / 2 <= finalPlatform.y + finalPlatform.height / 2 &&
            player.y + player.height / 2 >= finalPlatform.y - finalPlatform.height / 2)
        {

            player.velocityY = 0;
            player.onGround = true;
            player.y = finalPlatform.y + finalPlatform.height / 2 + player.height / 2;
            gameWon = true;
            return;
        }
    }

    player.onGround = false; // Assume player is in the air until proven otherwise
    for (auto& platform : platforms)
    {
        if (player.x < platform.x + platform.width / 2 &&
            player.x > platform.x - platform.width / 2 &&
            player.y + player.height / 2 >= platform.y - platform.height / 2 && // Player's head is at or above platform's bottom surface
            player.y + player.height / 2 < platform.y + platform.height / 2)  // Player's head is below the platform's top
        {
            // Player is touching the bottom of the platform
           // player.velocityX = 0;74
            player.velocityY = -abs(player.velocityY);
            //std::cout << "bbbbb" << std::endl;

        }
        if (player.x < platform.x + platform.width / 2 &&
            player.x > platform.x - platform.width / 2 &&
            player.y - player.height / 2 <= platform.y + platform.height / 2 &&
            player.y - player.height / 2 > platform.y - platform.height / 2)
        {

            /*   if (platform.isTeleporting)
              {
                  // Teleport player to a random platform
                  int randomIndex;
                  do {
                      randomIndex = rand() % platforms.size();
                  } while (&platforms[randomIndex] == &platform); // Avoid teleporting to the same platform

                  player.x = platforms[randomIndex].x;
                  player.y = platforms[randomIndex].y + platforms[randomIndex].height / 2 + player.height / 2;
                  player.velocityY = 0;
                  player.onGround = true;
                  std::cout << "Teleported to another platform!" << std::endl;
                  break;
              } */

              // If the player is on a different platform, count it as a successful jump
            if (!player.onGround && currentPlatform != &platform && platform.stat == 1)
            {
                successfulJumps++; // Increment successful jumps only when landing on a new platform
               // std::cout << "Successful jumps: " << successfulJumps << std::endl;
                currentPlatform = &platform; // Update the current platform

                platform.stat = 0;
                platform.durability--;
                // Check if player reaches 10 successful jumps
                if ((currentLevel == 1 && successfulJumps == 2) || (currentLevel == 2 && successfulJumps == 4) || (currentLevel == 3 && successfulJumps == 5) || (currentLevel == 4 && successfulJumps == 7) || (currentLevel == 5 && successfulJumps == 8))
                {
                    // nextLevel();
                    finalPlatformActive = true;
                    // platforms.clear();  // Stop rendering other platforms
                  //  std::cout << "Final platform activated! level up" << std::endl;
                   // std::cout << "Successful level: " << currentLevel << std::endl;
                }
            }

            // Check if the platform is a bouncing platform
            if (platform.isBouncing)
            {
                //  player.velocityY = 0.4f - player.velocityY * 0.1f; // Bouncing effect, boost based on fall speed
            }
            else
            {
                // player.velocityY = 0;
            }
            if (platform.durability <= 0)
            {
                // player.y = 0;
                // felt = true;
            }
            player.velocityY = 0;
            player.onGround = true;
            player.y = platform.y + platform.height / 2 + player.height / 2;
            break; // No need to check further platforms once collision is detected
        }
    }
}
void Game::reset()
{
    // Reset game state variables
    successfulJumps = 0;
    finalPlatformActive = false;
    gameWon = false;
    gameover = false;
    felt = false;
    // Reset final platform position
    Platform p = Platform(player.x, player.y - 100);
    p.width = platforms[0].width;
    p.speed = platforms[0].speed;
    platforms.push_back(p); // Position platform below player

    // std::cout << "You compl!"<< player.y << std::endl;

    finalPlatform.y = 0; // Reset the final platform's position if needed
}
float lerp(float start, float end, float t) {
    return start + t * (end - start);
}
void Game::checkGameOver(int WORLD_HEIGHT)
{
    // Check if player has touched the top of the screen
    if (!felt)
    {
        if (player.y + player.height / 2 >= WORLD_HEIGHT && player.onGround)
        {
            felt = true;
        }

        // Check if player has fallen below the bottom of the screen
        if (player.y < 0)
        {
            player.y = WORLD_HEIGHT / 3;
            gameover = true;
            float startX = player.x;
            float startY = player.y;

            // Target position (x, y)
            float endX = player.x + 500;
            float endY = player.y+ 500;

            // Time and speed parameters
            float time = 0.0f;  // Start at time 0
            float duration = 10.0f;  // Animation will take 2 seconds
            float deltaTime = 0.01f;  // Time step for each update

            while (time <= duration) {
                // Calculate t as a fraction of the animation duration (range 0 to 1)
                float t = time / duration;

                // Interpolate x and y positions
                float currentX = lerp(startX, endX, t);
                float currentY = lerp(startY, endY, t);
                player.x = currentX;
                // Output the current position (you would normally render the object here)

                // Update time
                time += deltaTime;

            }
            //felt = true;
        }
    }
}
void Game::initPlatforms(int WORLD_WIDTH, int WORLD_HEIGHT, int WINDOW_WIDTH,int WINDOW_HEIGHT)
{
    maxLevel = 5;
    currentLevel = 1;
    NUM_PLATFORMS = 80;
    finalPlatform.x = WORLD_WIDTH / 2;
    finalPlatform.y = 0;
    finalPlatform.speed = 0;
    finalPlatform.width = WORLD_WIDTH;
    finalPlatform.height = 70;

    platforms.push_back(Platform(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 100)); // Position first platform below player
    while (platforms.size() < NUM_PLATFORMS)
    {
        float randomX = rand() % (WORLD_WIDTH - 100); // Ensure platform stays within window width
        float randomY = rand() % (WORLD_HEIGHT - 50); // Ensure platform stays within window height

        bool tooClose = false;

        // Check if the new platform is too close to existing ones
        for (const auto& platform : platforms)
        {
            float distX = platform.x - randomX;
            float distY = platform.y - randomY;
            float distance = sqrt(distX * distX + distY * distY);

            if (distance < (150.0f)) {
                tooClose = true;
                break;
            }
        }

        // Only add the platform if it's not too close to others
        if (!tooClose)
        {
            bool isTeleporting = (rand() % 5 == 0); // 20% chance to be teleporting
            bool isBouncing = (rand() % 2 == 0);    // 50% chance to be a bouncing platform
            platforms.push_back(Platform(randomX, randomY, 100.0f, 20.0f, 50.0f, isBouncing, 2, isTeleporting));
        }
    }
}
void Game::initParts(int WORLD_WIDTH, int WORLD_HEIGHT) {
    srand(static_cast<unsigned>(time(0)));

    // Loop to render parts in random locations
    for (int i = 0; i < WORLD_WIDTH / 2; ++i)
    {
        float x = static_cast<float>(rand()) / RAND_MAX * WORLD_WIDTH;
        float y = static_cast<float>(rand()) / RAND_MAX * WORLD_HEIGHT;
        parts.push_back({ x, y });
    }
}
void Game::nextLevel()
{
    currentLevel++;
    platforms.erase(platforms.begin(), platforms.begin() + 15);

    // Increase platform difficulty
    for (auto& platform : platforms)
    {
        platform.speed += 30.0f; // Increase speed of platforms
        platform.width -= 10.0f; // Decrease platform size for difficulty
    }

    // Possibly add more platforms or new obstacles
    if (currentLevel == 3)
    {

        // Add a moving obstacle, or other challenge
    }

    if (currentLevel > maxLevel)
    {
        // Trigger final level or game completion
       // std::cout << "You completed the game!" << std::endl;
        finalPlatformActive = true;
        // gameWon = true;
    }
}