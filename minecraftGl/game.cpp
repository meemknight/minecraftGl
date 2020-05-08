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
#include "Entity.h"
#include "input.h"
#include "WorldGenerator.h"
#include <filesystem>
#include "opengl2Dlib.h"
#include "Ui.h"
#include "menu.h"

ShaderProgram sp;
ShaderProgram spNoTexture;

Texture bloc;
gl2d::Texture uiTexture;
gl2d::Texture uiBackgroundTexture;
gl2d::Font font;
TextureAtlas uiAtlas{6, 1};
int width;
int height;

static glm::vec2 oldMousePosition;

//Camera camera(60.f, &width, &height, 0.1, 200);
FirstPersonCamera camera(80.f, 0.1, 200, &width, &height);

CubeMeshRenderer cubeRenderer;

CubeWireRenderer cubeWireRenderer;

ChunkManager chunkManager;

ChunkFileHandler fileHandler;

std::vector<glm::ivec3> chunksToLoad;

Entity playerEntity;

gl2d::Renderer2D renderer2d;

int initGame()
{
	//renderer2d
	{
		renderer2d.create();


	}

	std::filesystem::remove_all("saves");
	std::filesystem::create_directory("saves");

	
	//playerEntity.flySpeed *= 10;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_SAMPLE_SHADING);
	glClearColor(35/256.f, 157/256.f, 194 / 256.f, 1);

	glLineWidth(4);

	sp = ShaderProgram("vertex.vert", "fragment.frag");
	spNoTexture = ShaderProgram("noTexture.vert", "noTexture.frag");

	bloc.create("textures/blocks.png");
	bloc.subDivisions = 16;

	uiTexture.loadFromFile("textures/ui0.png");
	uiBackgroundTexture.loadFromFile("textures/uiDialogFrame.png");
	font.createFromFile("textures/font.ttf");
	//uiTexture.loadMipmap("textures/ui1.png", 1);

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

	chunkManager.requestChunks(chunksToLoad.data(), chunksToLoad.size(), 1, { camera.position.x, camera.position.z });
	
	menu::resetMenuState();

	int yPos = 0;

	
	if(!fileHandler.loadPlayer(camera.position))
	{
		for (int y = BUILD_LIMIT; y > 1; y--)
		{
			if (isCollideble(chunkManager.getBlock({ 0,y,0 })))
			{
				break;
			}
			yPos = y;
		}

		camera.position.y = yPos;
		camera.speed *= 1;
		camera.position.x = 0;
		camera.position.z = 0;
	}


	playerEntity.lastPos = camera.position;
	playerEntity.position = camera.position;

	return 1;
}

static int mainMenu = 0;

int gameLogic(float deltaTime)
{


#pragma region init
	width = getWindowSizeX();
	height = getWindowSizeY();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, getWindowSizeX(), getWindowSizeY());
	renderer2d.updateWindowMetrics(width, height);

#pragma endregion


	if (mainMenu)
	{

		gl2d::enableNecessaryGLFeatures();

		showMouse(true);

		bool start = 0;
		menu::startMenu();

		menu::uninteractableCentreText("MinecraftGl");

		menu::interactableText("Start", &start);
		menu::interactableText("Start", &start);
		menu::interactableText("Start", &start);
		menu::interactableText("Start", &start);

		menu::endMenu(renderer2d, uiBackgroundTexture, font, nullptr, deltaTime);

		if (start)
		{
			mainMenu = false;
		}

		renderer2d.flush();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		return 1;
	}


#pragma region keys

	glm::vec2 moveDirection = input::getMoveDirection();

	if (input::isKeyHeld(input::Buttons::jump))
	{
		playerEntity.walk(camera).jump();
	}

	playerEntity.walk(camera).moveOnDirection(moveDirection.y, moveDirection.x, deltaTime);

	if (isKeyPressed('P'))
	{
		for (auto &i : chunkManager.loadedChunks)
		{
			i.shouldRecreate = true;
			for (int j = 0; j < 6; j++)
			{
				i.positionData[j].size = 0;
			}
		}
	}

	if(isKeyPressedOn('O'))
	{
		playerEntity.velocity.x = 100;
	}

	//compute mouse delta
	if (isFocused())
	{
		glm::vec2 delta = glm::vec2(getRelMousePosition()) - oldMousePosition;
		delta.x *= -1;
		delta.y *= -1;

		camera.updatePosition(input::getLookDirection() * glm::vec2{ 12,12 } +delta);

		setRelMousePosition(getWindowSizeX() / 2, getWindowSizeY() / 2);
		oldMousePosition = getRelMousePosition();

		showMouse(false);
	}else
	{
		showMouse(false);
	}


#pragma endregion

#pragma region player phisics

	playerEntity.applyGravity(deltaTime);

	playerEntity.applyVelocity(deltaTime);

	playerEntity.resolveConstrains(chunkManager, { 1, 1.9, 1 }, nullptr);
	playerEntity.updatePositions();

	camera.position = playerEntity.position;

#pragma endregion


#pragma region block placement

	std::optional<glm::ivec3> coll, edge;

	rayCastAdvanced(chunkManager, { camera.position.x + 0.5, camera.position.y + 0.5, camera.position.z + 0.5 }, camera.viewDirection, 25,
		coll, edge);

	static int  curentBlock = 1;
	
	if (input::isKeyPressedOn(input::Buttons::q))
	{
		curentBlock--;
	}
	if (input::isKeyPressedOn(input::Buttons::e))
	{
		curentBlock++;
	}

	if (curentBlock < 1) { curentBlock = BLOCK::BLOCKS_SIZE - 1; }
	if (curentBlock >= BLOCK::BLOCKS_SIZE) { curentBlock = 1; }

	//ilog(curentBlock);


	if (coll.has_value())
	{
		cubeWireRenderer.addCube({ coll.value() }, { 0.61,0.6,0.65,1 });
		//ilog(coll.value().x, coll.value().y, coll.value().z);

		if (coll && input::isKeyPressedOn(input::Buttons::placeBlock))
		{
			if (edge.has_value())
			{
		
				//generateStructure(chunkManager, 0, edge.value(), {}, true, false);
				chunkManager.setBlock(edge.value(), curentBlock);
			}
		}

		if (coll && input::isKeyPressedOn(input::Buttons::breakBlock))
		{
			chunkManager.setBlock(coll.value(), BLOCK::air);
		}

	}

#pragma endregion


#pragma region drawing

	cubeWireRenderer.draw();

	chunksToLoad.clear();

	camera.getChunksInFrustrum(chunksToLoad);
	Chunk **c = chunkManager.requestChunks(chunksToLoad.data(), chunksToLoad.size(), 1, { camera.position.x, camera.position.z });
	chunkManager.bakeUnbakedChunks(3, { camera.position.x, camera.position.z });

	//cubeRenderer.addSingleCube(0, 100, 0, BLOCK::gold_block);
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

#pragma region 2d

	gl2d::enableNecessaryGLFeatures();
	renderer2d.updateWindowMetrics(width, height);

	{
		Ui::Frame f({ 0,0, width, height});

		//renderer2d.renderRectangle(
		//	{ 100, 100, 400, 400 }, {0,1,1,0.2}, {}, 0
		//);

		renderer2d.renderRectangle(
			Ui::Box().xCenter().yCenter().xDimensionPixels(30).yAspectRatio(1.f), {}, 0,
			uiTexture, uiAtlas.get(2, 0)
		);


		auto box = Ui::Box().xLeft(20).yBottom(-20).
			yDimensionPercentage(0.1).xAspectRatio(1)();
		renderer2d.renderRectangle(box
			,
			{}, 0, uiTexture, uiAtlas.get(1, 0));
		{
			Ui::Frame f2{box};
			gl2d::Texture textures;
			textures.id = bloc.id;
			TextureAtlas TextureAtlas{ 16, 16 };

			auto face = getBlockFace(curentBlock, FACE::front);

			renderer2d.renderRectangle(Ui::Box().xCenter().yCenter().xDimensionPercentage(0.6f).yAspectRatio(1),
				{}, 180, textures, TextureAtlas.get(face.x, face.y));

		}



	}

	//renderer2d.clearScreen();
	//renderer2d.renderRectangle({ width / 2 - 25, height / 2 - 25,25,25 }, { 1,0,0,1 });
	//renderer2d.renderRectangle({ width / 2 - 10 , height / 2 - 25,25,25 }, { 0,1,0,0.2 });
	renderer2d.flush();
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

#pragma endregion


	return 1;
}

void closeGame()
{
	FreeConsole();

	fileHandler.savePlayer(camera.position);

	for(auto &i : chunkManager.loadedChunks)
	{
		if(i.shouldReSave && i.fullyLoaded)
		{
			fileHandler.saveChunk(i);
		}
	}
	Sleep(200);
}