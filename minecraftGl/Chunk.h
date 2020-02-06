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

	void clearBlockData() { memset(blockData, 0, sizeof(blockData)); }

	Block &getBlock(glm::ivec3 v) { return getBlock(v.x, v.y, v.z); }
	Block &getBlock(int x, int y, int z) { return blockData[x][z][y]; }
		
	//the position is rel to chunk mesh
	glm::ivec3 position;
	glm::ivec3 getPositionInUnits() { return { position.x * CHUNK_SIZE, position.y, position.z * CHUNK_SIZE }; }

	unsigned char shouldRecreate = 0;
	unsigned char shouldReSave = 0;

	Chunk *neighbours[4] = { 0 };

	FloatVector positionData[FACE::FACES_SIZE + 1] = {};

	//sets the positionData to 0
	void resetMeshes()
	{
		for (int i = 0; i < FACE::FACES_SIZE + 1; i++)
		{
			positionData[i].size = 0;
		}
	}

	void cleanup()
	{
		for (int i = 0; i < FACE::FACES_SIZE + 1; i++)
		{
			positionData[i].cleanup();
		}
	}

	void bakeMeshes();

	//looks in the neighbours and removes itself from their list.
	//it is used before deleting this chunk
	void removeNeighboursLinkage();

private:
	friend class ChunkFileHandler;
	//private because it is prone to change
	Block blockData[CHUNK_SIZE][CHUNK_SIZE][BUILD_LIMIT];
};

