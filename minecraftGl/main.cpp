#include "platformIndependentCalls.h"
#include "game.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <Windows.h>

static GLFWwindow* window;

static int xsize;
static int ysize;

int main()
{
	if (glfwInit() == false)
	{
		MessageBoxA(0, "glfwInit", "Error from glfw", MB_ICONERROR);
		return 1;
	}

	float width = 640;
	float heigth = 480;
	GLFWwindow* window = glfwCreateWindow(width, heigth, "Minicraft", NULL, NULL);
	
	if (!window)
	{
		MessageBoxA(0, "glfwCreateWindow", "Error from glfw", MB_ICONERROR);
		return 1;
	}

	glfwMakeContextCurrent(window);

	if(glewInit() != GLEW_OK)
	{
		MessageBoxA(0, "glewInit", "Error from glew", MB_ICONERROR);
		return 1;
	}

	initGame();

	while(!glfwWindowShouldClose(window))
	{
		//todo properly implement this
		glfwGetWindowSize(window, &xsize, &ysize);
		glfwPollEvents();
			
		gameLogic(0);

		glfwSwapBuffers(window);
	}
	
	return 0;
}

//platform independent functions implementation
///////////////////////////////////////////////
void errorMessage(const char *c) 
{
	MessageBoxA(0, c, "error", MB_ICONERROR);
}

//todo properly implement this
void setRelMousePosition(int x, int y)
{
	int winx = 0;
	int winy = 0;

	glfwGetWindowPos(window, &winx, &winy);

	SetCursorPos(x + winx, y + winy);
}

//todo properly implement this
glm::ivec2 getRelMousePosition()
{
	POINT p;
	GetCursorPos(&p);

	int winx = 0;
	int winy = 0;

	glfwGetWindowPos(window, &winx, &winy);

	p.x -= winx;
	p.y -= winy;

	p.x = max(0, p.x);
	p.y = max(0, p.y);

	return { p.x, p.y };
}

glm::ivec2 getWindowSize()
{
	return { xsize, ysize };
}