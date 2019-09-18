#pragma once
#include "Blocks.h"
#include "FloatVector.h"
#include <cstringt.h>
#include <glm/vec3.hpp>
constexpr int BUILD_LIMIT = 256;
constexpr int CHUNK_SIZE = 16;

//chunk neighbour
namespace CN
{
	enum
	{
		front = 0,
		back,
		left,
		right,
	};
}


struct Chunk
{
	Chunk() {}

	Block blockData[CHUNK_SIZE][BUILD_LIMIT][CHUNK_SIZE];

	void clear() { memset(blockData, 0, sizeof(blockData)); }

	Block &getBlock(glm::vec3 v) { return getBlock(v.x, v.y, v.z); }
	Block &getBlock(int x, int y, int z) { return blockData[x][y][z]; }
		
	glm::vec3 position;
	glm::vec3 getPositionInUnits() { return { position.x * CHUNK_SIZE, position.y, position.z * CHUNK_SIZE }; }

	int shouldRecreate = 0;

	Chunk *neighbours[4] = { 0 };

	FloatVector positionData[FACE::FACES_SIZE];

	//sets the positionData to 0
	void resetMeshes()
	{
		for (int i = 0; i < FACE::FACES_SIZE; i++)
		{
			positionData[i].size = 0;
		}
	}

	void cleanup()
	{
		for (int i = 0; i < FACE::FACES_SIZE; i++)
		{
			positionData[i].cleanup();
		}
	}

	void bakeMeshes();

	//looks in the neighbours and removes itself from their list.
	//it is used before deleting this chunk
	void removeNeighboursLinkage();
};

