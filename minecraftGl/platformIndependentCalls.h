#pragma once
#include <glm/glm.hpp>

//#define MGL_INDEPENDENT
#define MGL_WIN

void errorMessage(const char *c);

///sets the mouse pos relative to the window's drawing area
void setRelMousePosition(int x, int y);

///gets the mouse pos relative to the window's drawing area
glm::ivec2 getRelMousePosition();

//gets the drawing region sizes
glm::ivec2 getWindowSize();

inline int getWindowSizeX() { return getWindowSize().x; }
inline int getWindowSizeY() { return getWindowSize().y; }

int isKeyPressed(int key);
int isKeyPressedOn(int key);

int isLMouseButtonPressed();
int isRMouseButtonPressed();

int isLMouseHeld();
int isRMouseHeld();

void showMouse(bool show);