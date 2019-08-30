#pragma once
#include <glm/glm.hpp>

void errorMessage(const char *c);

///sets the mouse pos relative to the window's drawing area
void setRelMousePosition(int x, int y);

///gets the mouse pos relative to the window's drawing area
glm::ivec2 getRelMousePosition();

//gets the drawing region sizes
glm::ivec2 getWindowSize();

int getWindowSizeX() { return getWindowSize().x; }
int getWindowSizeY() { return getWindowSize().y; }