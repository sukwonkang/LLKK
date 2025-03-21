#pragma once
#ifndef GAME_H
#define GAME_H
#include <vector>
#include "parties/player.h"
#include "parties/platform.h"
#include "particule.h"
#include "camera.h"

struct Game {
	Player player;
	std::vector<Platform> platforms;
	std::vector<Part> parts;
	Platform finalPlatform;
	Platform* currentPlatform;
	int NUM_PLATFORMS;
	int successfulJumps;
	int currentLevel;
	int maxLevel;
	bool finalPlatformActive;
	bool gameWon;
	bool gameover; 
	bool felt;
	Game(Player player);
	void update();
	void checkCollision();	
	void reset();
	void checkGameOver(int WORLD_HEIGHT);
	void initPlatforms(int w,int h,int ww,int hh);
	void initParts(int WORLD_WIDTH, int WORLD_HEIGHT);
	void nextLevel();
};

#endif // PLATFORM_H