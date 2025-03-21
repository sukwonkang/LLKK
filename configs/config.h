#ifndef CONFIG_H
#define CONFIG_H


 float WINDOW_WIDTH = 1600;
 float WINDOW_HEIGHT = 800;

bool spacePressed = false;
bool downed = false;
bool upped = false;
bool menuVis = true;

bool movingLeft = false;
bool movingRight = false;


const int WORLD_WIDTH = 4000;
const int WORLD_HEIGHT = 2000;
int fps = 0;
bool fullscreen = false;
const int MAX_FPS = 140;
const double targetFrameTime = 1.0 / MAX_FPS; // Time for one frame in seconds


#endif // CONFIG_H
