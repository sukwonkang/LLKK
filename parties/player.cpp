
#include "player.h"

Player::Player(float xx,float yy) : x(xx), y(yy), width(50.0f), height(50.0f), velocityY(0.0f), velocityX(0.0f), depth(2.0f), z(1.0f), onGround(false) {}

void Player::jump() {
   if (onGround)
        {
            velocityY = 5.0f; // Jump velocity
            onGround = false; 
        }
}

void Player::update(float deltaTime,int WORLD_WIDTH,bool movingRight,bool movingLeft) {
    // Apply gravity
        velocityY -= 0.093f;
        y += velocityY; // Move player vertically

        // Keep applying horizontal velocity even in the air
        x += (velocityX)*deltaTime;

        if (velocityX > 0 && !onGround && movingRight)
        {
            x += (velocityX + 85) * deltaTime;
        }
        if (velocityX < 0 && !onGround && movingLeft)
        {
            x += (velocityX - 85) * deltaTime;
        }

        // Horizontal screen wrap-around
        if (x < 0)
            x = WORLD_WIDTH;
        if (x > WORLD_WIDTH)
            x = 0;

        // Reset player velocity if on the ground
        if (onGround)
        {
            velocityX *= 0.9f; // Apply slight friction when on the ground
        }
}
