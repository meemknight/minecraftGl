#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Blocks.h"
#include "FloatVector.h"
#include "Chunk.h"

struct BlockWithPos
{
	glm::vec3 pos;
	Block b;
};

struct CubeMeshRenderer
{
	CubeMeshRenderer() {};
	
	ShaderProgram *sp;
	//todo remove this
	FirstPersonCamera *camera;
	Texture *texture;

	GLuint facesBuffer[FACE::FACES_SIZE];
	GLuint facesIndexBuffer[FACE::FACES_SIZE];
	GLuint positionsBuffer[FACE::FACES_SIZE];
	GLuint vertexArrays[FACE::FACES_SIZE];

	GLuint herbsShapeData;
	GLuint herbsShapeIndexBuffer;
	GLuint herbsVertexArray;
	GLuint herbsPositionData;

	FloatVector positionData[FACE::FACES_SIZE];

	std::vector<BlockWithPos> additionalBlocks;

	void draw(Chunk **c, int size);
	void cleanup();
	void create();//todo rename init

	void addSingleCube(int x, int y, int z, Block type);
	void addSingleCube(glm::vec3 pos, Block type) { addSingleCube(pos.x, pos.y, pos.z, type); }

	GLint textureUniformLocation = -1;
	GLint matUniformLocation = -1;
	GLint indexUniformLocation = -1;
	GLint magnifierUniformLocation = -1;
	GLint ambienceUniformLocation = -1;
	GLint distanceUniformLocation = -1;
};
