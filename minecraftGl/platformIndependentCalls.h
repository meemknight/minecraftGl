#pragma once
#include <glm/glm.hpp>

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

int isLMouseButtonPressed();
int isRMouseButtonPressed();

void showMouse(bool show);