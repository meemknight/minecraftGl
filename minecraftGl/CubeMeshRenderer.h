#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Blocks.h"
#include "FloatVector.h"
#include "Chunk.h"

struct CubeMeshRenderer
{
	CubeMeshRenderer() {};
	
	ShaderProgram *sp;
	FirstPersonCamera *camera;
	Texture *texture;

	GLuint facesBuffer[FACE::FACES_SIZE];
	GLuint facesIndexBuffer[FACE::FACES_SIZE];
	GLuint positionsBuffer[FACE::FACES_SIZE];
	GLuint vertexArrays[FACE::FACES_SIZE];

	FloatVector positionData[FACE::FACES_SIZE];

	void draw(Chunk &c);
	void cleanup();
	void create();

	GLint textureUniformLocation = -1;
	GLint matUniformLocation = -1;
	GLint indexUniformLocation = -1;
	GLint magnifierUniformLocation = -1;
};

