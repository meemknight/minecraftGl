#include "game.h"

#include <GL/glew.h>

int initGame()
{
	
	return 1;
}

int gameLogic(float deltaTime)
{
	glBegin(GL_TRIANGLES);
	glVertex2f(0, 1);
	glVertex2f(-1, 0);
	glVertex2f(1, 0);
	glEnd();

	return 1;
}
