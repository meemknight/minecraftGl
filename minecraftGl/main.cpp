#include "platformIndependentCalls.h"
#include "game.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <Windows.h>

static GLFWwindow *window;

static int xsize;
static int ysize;

void windowSizeCallback(GLFWwindow *window, int x, int y)
{
	xsize = x;
	ysize = y;
}

int main()
{
	if (glfwInit() == false)
	{
		MessageBoxA(0, "glfwInit", "Error from glfw", MB_ICONERROR);
		return 1;
	}

	float width = 640;
	float heigth = 480;
	window = glfwCreateWindow(width, heigth, "Minicraft", NULL, NULL);

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

	glfwSetWindowSizeCallback(window, windowSizeCallback);
	glfwGetWindowSize(window, &xsize, &ysize);

	int time1 = GetTickCount();
	int time2 = GetTickCount();

	while(!glfwWindowShouldClose(window))
	{

		time2 = GetTickCount();
		int deltaTime = time2 - time1;
		time1 = GetTickCount();

		glfwPollEvents();
			
		gameLogic((float)deltaTime/1000.f);

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	
	return 0;
}

//platform independent functions implementation
///////////////////////////////////////////////
void errorMessage(const char *c) 
{
	MessageBoxA(0, c, "error", MB_ICONERROR);
}


void setRelMousePosition(int x, int y)
{
	glfwSetCursorPos(window, x, y);
}

glm::ivec2 getRelMousePosition()
{
	double cursorx;
	double cursory;

	glfwGetCursorPos(window, &cursorx, &cursory);

	return { (int)cursorx, (int)cursory };
}

glm::ivec2 getWindowSize()
{
	return { xsize, ysize };
}

int isKeyPressed(int key)
{
	return glfwGetKey(window, key);
}

void showMouse(bool show)
{

}
