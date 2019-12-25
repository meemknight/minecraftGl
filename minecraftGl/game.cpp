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
#include "fileHandler.h"
#include <filesystem>

ShaderProgram sp;
ShaderProgram spNoTexture;

Texture bloc;
int width;
int height;

//Camera camera(60.f, &width, &height, 0.1, 200);
FirstPersonCamera camera(60.f, 0.1, 200, &width, &height);

CubeMeshRenderer cubeRenderer;

CubeWireRenderer cubeWireRenderer;

ChunkManager chunkManager;

ChunkFileHandler fileHandler;

std::vector<glm::ivec3> chunksToLoad;

glm::vec3 playerLastPos;

int initGame()
{
	std::filesystem::remove_all("saves");
	std::filesystem::create_directory("saves");

	camera.position.y = 150;
	camera.speed *= 1;
	camera.position.x = 0;
	camera.position.z = 0;

	playerLastPos = camera.position;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	//glEnable(GL_SAMPLE_SHADING);

	glLineWidth(4);

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

	if (isKeyPressed('P'))
	{
		for(auto &i :chunkManager.loadedChunks)
		{
			i.shouldRecreate = true;
			for(int j=0;j<6;j++)
			{
				i.positionData[j].size = 0;
			}
		}
	}

	camera.mouseUpdate(getRelMousePosition());

#pragma endregion


#pragma region player phisics
	
	resolveConstrains(camera.position, playerLastPos, chunkManager, { 1, 1.9, 1}, nullptr);
	playerLastPos = camera.position;

#pragma endregion


#pragma region block placement

	std::optional<glm::ivec3> coll, edge;

	rayCastAdvanced(chunkManager, { camera.position.x + 0.5, camera.position.y + 0.5, camera.position.z + 0.5 }, camera.viewDirection, 25,
		coll, edge);

	if (coll.has_value())
	{
		cubeWireRenderer.addCube({ coll.value() }, { 0.61,0.6,0.65,1 });
		//ilog(coll.value().x, coll.value().y, coll.value().z);

		if (coll && isRMouseButtonPressed())
		{
			if (edge.has_value())
			{
				chunkManager.setBlock(edge.value(), BLOCK::leaves);
			}
		}

		if (coll && isLMouseButtonPressed())
		{
			chunkManager.setBlock(coll.value(), BLOCK::air);
		}

	}

#pragma endregion


#pragma region drawing

	cubeWireRenderer.draw();

	chunksToLoad.clear();

	camera.getChunksInFrustrum(chunksToLoad);
	Chunk **c = chunkManager.requestChunks(chunksToLoad.data(), chunksToLoad.size());
	chunkManager.bakeUnbakedChunks(4, { camera.position.x, camera.position.z });

	cubeRenderer.addSingleCube(0, 100, 0, BLOCK::gold_block);
	cubeRenderer.draw(c, chunksToLoad.size());

	//glUseProgram(0);
	//glDisable(GL_DEPTH_TEST);
	//glBegin(GL_TRIANGLES);
	//glVertex3f(0.f, 0.005f, 0);
	//glVertex3f(-0.005f, -0.005f,0);
	//glVertex3f(0.005f, -0.005f,0);
	//glEnd();
	//glEnable(GL_DEPTH_TEST);

#pragma endregion

	//llog((camera.position.x), (camera.position.y), (camera.position.z));
	//llog(camera.viewDirection.x, camera.viewDirection.y, camera.viewDirection.z);

	return 1;
}

void closeGame()
{
	for(auto &i : chunkManager.loadedChunks)
	{
		if(i.shouldReSave)
		{
			fileHandler.saveChunk(i);
		}
	}
	Sleep(2000);
}