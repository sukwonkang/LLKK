#pragma once
#ifndef GPH_H
#define GPH_H
#include <GL/glew.h>
#include <vector>
#include "parties/player.h"
#include "parties/platform.h"



struct GPH {
	GPH();
	void renderPlayer(GLuint textureID,Player& player);
	void renderPlatform(GLuint textureID, Platform& plat);
	void drawStripedBorder(float worldWidth, float worldHeight, float borderThickness, int numStripes);
	void renderPart(float x, float y, float rectWidth, float rectHeight);
	GLuint loadTexture(const char* path);
	void drCamera(int windowWidth, int windowHeight, float x, float y, float zoom);
};

#endif