#ifndef PLATFORM_H
#define PLATFORM_H
#include <vector>

struct Platform {
    float x, y;
    float width, height;
    float speed; // Platform movement speed (in pixels per second)
    bool isBouncing; // New flag to identify bouncing platforms
    int durability; // Number of times the platform can be stepped on before breaking
    bool isTeleporting; // New flag for teleporting platforms
    float depth = 2;
    int stat = 1;
    Platform();
    Platform(float xx,float yy);
    Platform(float xx,float yy,float width,float height,float speed,bool isBouncing,int durability, bool isTeleporting);
    void update(float deltatime,int w,int h, std::vector<Platform> platforms);
};

#endif // PLATFORM_H