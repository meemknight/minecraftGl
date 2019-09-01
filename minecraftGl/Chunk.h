#pragma once
#include "Blocks.h"
#include <cstringt.h>
#include <glm/vec3.hpp>
constexpr int BUILD_LIMIT = 256;
constexpr int CHUNK_SIZE = 16;

struct Chunk
{
public:
	Chunk() {}

	Block blockData[CHUNK_SIZE][BUILD_LIMIT][CHUNK_SIZE];

	void clear() { memset(blockData, 0, sizeof(blockData)); }

	Block &getBlock(glm::vec3 v) { return getBlock(v.x, v.y, v.z); }
	Block &getBlock(int x, int y, int z) { return blockData[x][y][z]; }
		
};

