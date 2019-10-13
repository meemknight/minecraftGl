#include "platformIndependentCalls.h"
#include "game.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <ctime>

#include "tools.h"

extern "C"
{
	//__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
	//__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

static GLFWwindow *window;

static int xsize;
static int ysize;

static int lbutton = 0;
static int rbutton = 0;

static int lbuttonPressed = 0;
static int rbuttonPressed = 0;

void windowSizeCallback(GLFWwindow *window, int x, int y)
{
	xsize = x;
	ysize = y;
}

void windowMouseCallback(GLFWwindow*, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		lbutton = 1;
		lbuttonPressed = 1;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		rbutton = 1;
		rbuttonPressed = 1;
	}

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		lbutton = 0;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		rbutton = 0;
	}
}


int main()
{
	if (glfwInit() == false)
	{
		MessageBoxA(0, "glfwInit", "Error from glfw", MB_ICONERROR);
		return 1;
	}

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	float width = 960;
	float heigth = 680;
	
	//glfwWindowHint(GLFW_SAMPLES, 4);

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

	if(!initGame())
	{
		return 0;
	}

	{
		glfwSetWindowSizeCallback(window, windowSizeCallback);
		glfwGetWindowSize(window, &xsize, &ysize);

		glfwSetMouseButtonCallback(window, windowMouseCallback);

		int time1 = clock();
		int time2 = clock();

		ShowCursor(0);
		Sleep(1000);
		while (!glfwWindowShouldClose(window))
		{
			lbuttonPressed = 0;
			rbuttonPressed = 0;

			time2 = clock();
			int deltaTime = time2 - time1;
			time1 = clock();
		
			glfwPollEvents();
			
			if (!gameLogic((float)deltaTime / CLOCKS_PER_SEC))
			{
				break;
			}

			llog(1.f/(deltaTime / 1000.f));

			glfwSwapBuffers(window);
		}

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

int isLMouseButtonPressed()
{
	return lbuttonPressed;
}

int isRMouseButtonPressed()
{
	return rbuttonPressed;
}

int isLMouseHeld()
{
	return lbutton;
}

int isRMouseHeld()
{
	return rbutton;
}

void showMouse(bool show)
{

}
