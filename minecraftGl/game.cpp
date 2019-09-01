#include "game.h"

#include <GL/glew.h>
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "CubeMeshRenderer.h"

static GLuint quadBuff;
static GLuint indexBuffer;
ShaderProgram sp;

float quadData[] =
{
	-0.8f, 0.8f,//pos
	 0.0f, 1.0f,//texcoord
	
	-0.8f, -0.8f,
	 0.0f,  0.0f,

	 0.8f, -0.8f,
	 1.0f,  0.0f,

	 0.8f, 0.8f,
	 1.0f, 1.0f,
};


unsigned int indexBufferData[] =
{
	0,
	1,
	3,
	3,
	1,
	2,
};

Texture bloc;
int width;
int height;

//Camera camera(60.f, &width, &height, 0.1, 200);
FirstPersonCamera camera(60.f, 0.1, 200, &width, &height);

CubeMeshRenderer cubeRenderer;

int initGame()
{
	glGenBuffers(1, &quadBuff);
	glBindBuffer(GL_ARRAY_BUFFER, quadBuff);
	glNamedBufferData(quadBuff, sizeof(quadData), quadData, GL_STATIC_DRAW);

	glCreateBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glNamedBufferData(indexBuffer, sizeof(indexBufferData), indexBufferData, GL_STATIC_DRAW);

	sp = ShaderProgram("vertex.vert", "fragment.frag");
	sp.bind();

	bloc.create("textures/blocks.png");
	bloc.subDivisions = 16;

	cubeRenderer.camera = &camera;
	cubeRenderer.texture = &bloc;
	cubeRenderer.sp = &sp;
	cubeRenderer.create();

	return 1;
}

int gameLogic(float deltaTime)
{
#pragma region init
	width = getWindowSizeX();
	height = getWindowSizeY();
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, getWindowSizeX(), getWindowSizeY());
#pragma endregion

#pragma region keys
	if (isKeyPressed('W'))
	{
		camera.moveFront(deltaTime);
	}

	if (isKeyPressed('A'))
	{
		camera.moveLeft(deltaTime);
	}

	if (isKeyPressed('S'))
	{
		camera.moveBack(deltaTime);
	}

	if (isKeyPressed('D'))
	{
		camera.moveRight(deltaTime);
	}

	if (isKeyPressed('R'))
	{
		camera.moveUp(deltaTime);
	}

	if (isKeyPressed('F'))
	{
		camera.moveDown(deltaTime);
	}

	camera.mouseUpdate(getRelMousePosition());

#pragma endregion

	cubeRenderer.draw();

	return 1;
}
