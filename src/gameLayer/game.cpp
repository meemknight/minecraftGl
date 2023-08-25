#include "game.h"

#include <glad/glad.h>
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "CubeMeshRenderer.h"
#include "ChunkManager.h"
#include <stb_image/stb_image.h>
#include "phisics.h"
#include "debugTools.h"
#include "fileHandler.h"
#include "Entity.h"
#include "WorldGenerator.h"
#include <filesystem>
#include <gl2d/gl2d.h>
#include "Ui.h"
#include "SkyBox.h"
#include "platformInput.h"

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

FirstPersonCamera camera(80.f, 0.1, 16 * 2, &width, &height);

CubeMeshRenderer cubeRenderer;

CubeWireRenderer cubeWireRenderer;

ChunkManager chunkManager;

ChunkFileHandler fileHandler;

std::vector<glm::ivec3> chunksToLoad;

Entity playerEntity;

gl2d::Renderer2D renderer2d;

SkyBox skyBox;

bool initGame(gl2d::FrameBuffer fbo)
{
	//renderer2d
	{
		renderer2d.create(fbo.fbo);

	}

	std::filesystem::create_directory(RESOURCES_PATH "saves");
	//std::filesystem::remove_all("saves");

	//playerEntity.flySpeed *= 10;
	//glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	//glEnable(GL_POINT_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_MULTISAMPLE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);

	//glEnable(GL_SAMPLE_SHADING);
	glClearColor(35/256.f, 157/256.f, 194 / 256.f, 1);

	//glLineWidth(4);

	sp = ShaderProgram(RESOURCES_PATH "vertex.vert", RESOURCES_PATH  "fragment.frag");
	spNoTexture = ShaderProgram(RESOURCES_PATH  "noTexture.vert", RESOURCES_PATH  "noTexture.frag");

	bloc.create(RESOURCES_PATH "textures/blocks.png");
	bloc.subDivisions = 16;

	uiTexture.loadFromFile(RESOURCES_PATH "textures/ui0.png");
	uiBackgroundTexture.loadFromFile(RESOURCES_PATH "textures/uiDialogFrame.png");
	font.createFromFile(RESOURCES_PATH "textures/font.ttf");

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
	
	int yPos = 0;

	
	if(!fileHandler.loadPlayer(camera.position))
	{
		
		chunkManager.bakeUnbakedChunks(3, 9, { camera.position.x, camera.position.z });


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
	//playerEntity.flySpeed *= 10;
	//playerEntity.jumpSpeed *= 10;

	skyBox.createGpuData();
	skyBox.loadTexture(RESOURCES_PATH  "textures/sky.png");


	return 1;
}


bool gameLogic(float deltaTime, gl2d::FrameBuffer fbo)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquation(GL_FUNC_ADD);


#pragma region init
	width = platform::getFrameBufferSizeX();
	height = platform::getFrameBufferSizeY();

	width = std::max(1, width);
	height = std::max(1, height);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo.fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);
	renderer2d.updateWindowMetrics(width, height);

#pragma endregion




#pragma region keys



	glm::vec2 moveDirection = {};

	if (platform::isKeyHeld(platform::Button::W))
	{
		moveDirection.y += 1;
	}
	if (platform::isKeyHeld(platform::Button::S))
	{
		moveDirection.y -= 1;
	}
	if (platform::isKeyHeld(platform::Button::A))
	{
		moveDirection.x += 1;
	}
	if (platform::isKeyHeld(platform::Button::D))
	{
		moveDirection.x -= 1;
	}
	moveDirection = glm::normalize(moveDirection);

	if (platform::isKeyHeld(platform::Button::Space))
	{
		playerEntity.walk(camera).jump();
	}

	playerEntity.walk(camera).moveOnDirection(moveDirection.y, moveDirection.x, deltaTime);

	if (platform::isKeyHeld(platform::Button::P))
	{
		for (auto &i : chunkManager.loadedChunks)
		{
			i.shouldRecreate = true;
			i.resetMeshes();
			
		}
	}

	for (int i = 0; i < 9; i++)
	{
		if(platform::isKeyPressedOn(platform::Button::NR1+i))
		{
			camera.farPlane = (i + 1)* 5 * CHUNK_SIZE;
		}

	}

	if(platform::isKeyPressedOn(platform::Button::O))
	{
		playerEntity.velocity.x = 100;
	}

	//compute mouse delta
	if (platform::isFocused())
	{
		glm::vec2 delta = glm::vec2(platform::getRelMousePosition()) - oldMousePosition;
		delta.x *= -1;
		delta.y *= -1;

		camera.updatePosition(delta);

		platform::setRelMousePosition(width / 2, height / 2);
		oldMousePosition = platform::getRelMousePosition();

		platform::showMouse(false);
	}else
	{
		platform::showMouse(true);
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
	
	if (platform::isKeyPressedOn(platform::Button::Q))
	{
		curentBlock--;
	}
	if (platform::isKeyPressedOn(platform::Button::E))
	{
		curentBlock++;
	}

	if (curentBlock < 1) { curentBlock = BLOCK::BLOCKS_SIZE - 1; }
	if (curentBlock >= BLOCK::BLOCKS_SIZE) { curentBlock = 1; }

	//ilog(curentBlock);


	if (coll.has_value())
	{
		//cubeWireRenderer.addCube({ coll.value() }, { 0.61,0.6,0.65,1 });
		cubeRenderer.addSingleCube(coll.value().x, coll.value().y, coll.value().z
			, BLOCK::borderBlock);

		//ilog(coll.value().x, coll.value().y, coll.value().z);

		if (coll && platform::isRMousePressed())
		{
			if (edge.has_value())
			{
		
				//generateStructure(chunkManager, 0, edge.value(), {}, true, false);
				chunkManager.setBlock(edge.value(), curentBlock);
			}
		}

		if (coll && platform::isLMousePressed())
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
	chunkManager.bakeUnbakedChunks(1, 1, { camera.position.x, camera.position.z });

	//cubeRenderer.addSingleCube(0, 100, 0, BLOCK::gold_block);
	cubeRenderer.draw(c, chunksToLoad.size(), skyBox);

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
	
#pragma region skyBox
	{
		auto projMat = camera.getProjectionMatrix();
		auto viewMat = camera.getObjectToWorldMatrix();
		viewMat = glm::mat4(glm::mat3(viewMat));

		auto viewProjMat = projMat * viewMat;

		skyBox.draw(viewProjMat);
	}
#pragma endregion


#pragma region 2d

	if (1)
	{

		gl2d::enableNecessaryGLFeatures();
		renderer2d.updateWindowMetrics(width, height);

		{
			Ui::Frame f({0,0, width, height});

			//renderer2d.renderRectangle(
			//	{ 100, 100, 400, 400 }, {0,1,1,0.2}, {}, 0
			//);

			renderer2d.renderRectangle(
				Ui::Box().xCenter().yCenter().xDimensionPixels(30).yAspectRatio(1.f),
				uiTexture, uiAtlas.get(2, 0)
			);


			auto box = Ui::Box().xLeft(20).yBottom(-20).
				yDimensionPercentage(0.1).xAspectRatio(1)();
			renderer2d.renderRectangle(box,
				uiTexture, uiAtlas.get(1, 0));
			{
				Ui::Frame f2{box};
				gl2d::Texture textures;
				textures.id = bloc.id;
				TextureAtlas TextureAtlas{16, 16};

				auto face = getBlockFace(curentBlock, FACE::front);

				renderer2d.renderRectangle(Ui::Box().xCenter().yCenter().xDimensionPercentage(0.6f).yAspectRatio(1),
					textures, Colors_White, {}, 180, TextureAtlas.get(face.x, face.y));

			}

			//ui 
			{
				auto box2 = Ui::Box().xLeft(20).yTop(0).
					yDimensionPercentage(0.5).xAspectRatio(1)();

				Ui::Frame f2{box2};

				char buf[100] = {};
				sprintf(buf, "x:%+.3f y:%+.3f z:%+.3f",
					playerEntity.position.x,
					playerEntity.position.y,
					playerEntity.position.z);

				if (0)
				{
					renderer2d.renderText(Ui::Box().xLeft().yDistancePixels(30)(), buf, font,
						Colors_White, 0.35, 4, 3, false);

					sprintf(buf, "View distance: %.3f chunks.",
						camera.farPlane / 16.0);
					renderer2d.renderText(Ui::Box().xLeft().yDistancePixels(60)(), buf, font,
						Colors_White, 0.35, 4, 3, false);

					//sprintf(buf, "fps: %d",
					//	gameData.fps);
					//renderer2d.renderText(Ui::Box().xLeft().yDistancePixels(90)(), buf, font,
					//	Colors_White, 0.35, 4, 3, false);

					//sprintf(buf, "Renderer: %s",
					//	gameData.renderer);
					//renderer2d.renderText(Ui::Box().xLeft().yDistancePixels(120)(), buf, font,
					//	Colors_White, 0.35, 4, 3, false);
				}
			}

		}

		//renderer2d.clearScreen();
		//renderer2d.renderRectangle({ width / 2 - 25, height / 2 - 25,25,25 }, { 1,0,0,1 });
		//renderer2d.renderRectangle({ width / 2 - 10 , height / 2 - 25,25,25 }, { 0,1,0,0.2 });
		renderer2d.flush();
		//glDisable(GL_BLEND);
		//glEnable(GL_DEPTH_TEST);

	};
	

#pragma endregion


	return 1;
}

void closeGame()
{
	FreeConsole();

	fileHandler.savePlayer(camera.position);

	for(auto &i : chunkManager.loadedChunks)
	{
		//if (i.shouldReSave && i.structuresLoaded)
		if(i.shouldReSave)
		{
			fileHandler.saveChunk(i);
		}
	}
	Sleep(200);
}