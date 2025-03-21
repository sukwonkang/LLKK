
#include "camera.h"
Camera::Camera() { x = 0; y = 0; zoom = 1.0f; }


void Camera::update(float WINDOW_WIDTH, float WINDOW_HEIGHT, float WORLD_WIDTH, float WORLD_HEIGHT, float playerx,float playery) {
    // Calculate the visible area width and height based on the zoom factor
    float visibleWidth = WINDOW_WIDTH / zoom;
    float visibleHeight = WINDOW_HEIGHT / zoom;

    // Keep the player centered, adjusting for zoom
    x = playerx - visibleWidth / 2;
    y = playery - visibleHeight / 2;

    // Clamp the camera within the world boundaries, adjusted for zoom
    x = std::max(0.0f, std::min(x, WORLD_WIDTH - visibleWidth));
    y = std::max(0.0f, std::min(y, WORLD_HEIGHT - visibleHeight));
}

void Camera::setZoom(bool up)
{
    up ? zoom = zoom + 0.1f : zoom = zoom - 0.1f;
    zoom = std::max(0.4f, std::min(zoom, 5.0f)); // Clamp zoom to a reasonable range
}
