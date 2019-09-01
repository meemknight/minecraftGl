#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

enum FACES
{
	front = 0,
	back,
	top,
	bottom,
	left,
	right,
	FACES_SIZE,
};


struct CubeMeshRenderer
{
	CubeMeshRenderer();
	
	ShaderProgram *sp;
	FirstPersonCamera *camera;
	Texture *texture;

	GLuint facesBuffer[FACES::FACES_SIZE];
	GLuint facesIndexBuffer[FACES::FACES_SIZE];
	GLuint vertexArrays[FACES::FACES_SIZE];

	void draw();
	void cleanup();
	void create();

	GLint textureUniformLocation = -1;
	GLint matUniformLocation = -1;
	GLint indexUniformLocation = -1;
	GLint magnifierUniformLocation = -1;
};

