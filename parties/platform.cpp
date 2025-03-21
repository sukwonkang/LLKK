#include "platform.h"
#include <vector>
#include <stdlib.h>

Platform::Platform() : x(0), y(0), width(100.0f), height(20.0f), speed(50.0f), isBouncing(false), durability(2), isTeleporting(false) {}
Platform::Platform(float xx,float yy) : x(xx), y(yy),width(100.0f), height(20.0f), speed(50.0f), isBouncing(false), durability(2), isTeleporting(false){}
Platform::Platform(float xx,float yy,float width,float height,float speed,bool isBouncing,int durability, bool isTeleporting) :
 x(xx), y(yy), width(width), height(height), speed(speed), isBouncing(isBouncing), durability(durability), isTeleporting(isTeleporting) {}

void Platform::update(float deltaTime, int WORLD_HEIGHT,int WORLD_WIDTH, std::vector<Platform> platforms) {
      // Move platform upwards based on speed and deltaTime
        y += speed * deltaTime;

        // Reset the platform to the bottom if it moves out of the top of the screen
        if (y > WORLD_HEIGHT)
        {
            bool  tooClose = false;
            float yy = 0;                     // Bring platform back to the bottom
            float xx = rand() % WORLD_WIDTH; // Randomize x position again
            // Check if the new platform is too close to existing ones
            for (const auto& platform : platforms)
            {
                float distX = platform.x - xx;
                float distY = platform.y - yy;
                float distance = sqrt(distX * distX + distY * distY);

                if (distance < (150.0f)) {
                    tooClose = true;
                    break;
                }
            }
            if(!tooClose) {
                x = xx;
                y = yy;
            isBouncing = (rand() % 6 == 0); // 50% chance to be a bouncing platform
            isTeleporting = (rand() % 6 == 0); // 20% chance to be teleporting
            }

        }
}