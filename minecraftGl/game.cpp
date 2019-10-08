#include "game.h"

#include <GL/glew.h>
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "CubeMeshRenderer.h"
#include "ChunkManager.h"
#include "stb_image.h"
#include "phisics.h"
#include "debugTools.h"

static GLuint quadBuff;
static GLuint indexBuffer;
ShaderProgram sp;
ShaderProgram spNoTexture;


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
FirstPersonCamera camera(60.f, 0.1, 300, &width, &height);

CubeMeshRenderer cubeRenderer;

CubeWireRenderer cubeWireRenderer;

ChunkManager chunkManager;

std::vector<glm::ivec3> chunksToLoad;

glm::vec3 playerLastPos;

int initGame()
{
	camera.position.y = 100;
	camera.speed *= 0.4;

	playerLastPos = camera.position;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_MULTISAMPLE);
	//glEnable(GL_SAMPLE_SHADING);

	glLineWidth(4);

	glGenBuffers(1, &quadBuff);
	glBindBuffer(GL_ARRAY_BUFFER, quadBuff);
	glNamedBufferData(quadBuff, sizeof(quadData), quadData, GL_STATIC_DRAW);

	glCreateBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glNamedBufferData(indexBuffer, sizeof(indexBufferData), indexBufferData, GL_STATIC_DRAW);

	sp = ShaderProgram("vertex.vert", "fragment.frag");
	spNoTexture = ShaderProgram("noTexture.vert", "noTexture.frag");

	bloc.create("textures/blocks.png");
	bloc.subDivisions = 16;

	cubeRenderer.camera = &camera;
	cubeRenderer.texture = &bloc;
	cubeRenderer.sp = &sp;
	cubeRenderer.create();
	
	cubeWireRenderer.init(&spNoTexture, &camera);

	for(int i=0; i<40; i++)
	{
		for(int j=0;j<40; j++)
		{
			chunksToLoad.emplace_back(i-20,0,j-20);
		}
	}

	//drawWireCube(&camera, nullptr, {0,0,0});

	chunkManager.requestChunks(chunksToLoad.data(), chunksToLoad.size());

	return 1;
}


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


#pragma region player phisics
	
	resolveConstrains(camera.position, playerLastPos, chunkManager, { 0.5,2,0.5 }, &cubeWireRenderer);
	playerLastPos = camera.position;

#pragma endregion


#pragma region drawing
	chunksToLoad.clear();

	camera.getChunksInFrustrum(chunksToLoad);
	Chunk **c = chunkManager.requestChunks(chunksToLoad.data(), chunksToLoad.size());
	chunkManager.bakeUnbakedChunks(30);

	cubeRenderer.draw(c, chunksToLoad.size());

	cubeWireRenderer.addCube({ 0, 99, 0 }, { 0,0,1,1 });

	cubeWireRenderer.draw();
#pragma endregion


	//llog(floor(camera.position.x/16), floor(camera.position.z/16));
	llog(camera.position.x, camera.position.y, camera.position.z);


	return 1;
}
