#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#include <vector>

struct Camera {
    float x, y;
    float zoom; // Add a zoom factor
    Camera();
    void update(float WINDOW_WIDTH, float WINDOW_HEIGHT, float WORLD_WIDTH, float WORLD_HEIGHT, float playerx, float playery);
    void setZoom(bool up);
};

#endif // PLATFORM_H