#pragma once

struct GameData
{
	int fps;
	const unsigned char *vendor;
	const unsigned char *renderer;

};



int initGame();
int gameLogic(float deltaTime, GameData &gameData);
void closeGame();