#include "game.h"

#include <GL/glew.h>
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "CubeMeshRenderer.h"
#include "ChunkManager.h"
#include "stb_image.h"

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

ChunkManager chunkManager;

std::vector<glm::vec3> chunksToLoad;

int initGame()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

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
	
	chunksToLoad.reserve(100);

	return 1;
}

int distance = 6;

int gameLogic(float deltaTime)
{
#pragma region init
	width = getWindowSizeX();
	height = getWindowSizeY();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

	chunksToLoad.clear();
	//int posx = floor(camera.position.x / 16);
	//int posz = floor(camera.position.z / 16);
	//
	//for(int x=-distance; x<distance; x++)
	//{
	//	for(int z=-distance;z<distance;z++)
	//	{
	//		chunksToLoad.push_back({ (int)(posx + x), 0, (int)(posz + z)});
	//	}
	//}

	//llog(glm::degrees(camera.getTopDownAngle()));

	camera.getChunksInFrustrum(chunksToLoad);
	//llog(chunksToLoad.size());

	Chunk **c = chunkManager.requestChunks(chunksToLoad.data(), chunksToLoad.size());

	cubeRenderer.draw(c, chunksToLoad.size());


	//llog(floor(camera.position.x/16), floor(camera.position.z/16));

	return 1;
}
