#pragma once
#include <glm/vec2.hpp>

bool openTheNotepadInstance();

bool checkNotepadOpen();

void UpdateText();

glm::ivec2 getNotepadBufferSize();

void writeInBuffer(glm::ivec2 pos, char c);
void writeInBuffer(int x, int y, char c);

void writeInBuffer(glm::ivec2 pos, char c, char c2);
void writeInBuffer(int x, int y, char c, char c2);