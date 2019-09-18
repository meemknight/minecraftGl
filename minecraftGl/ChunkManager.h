#pragma once
#include "Chunk.h"
#include "glm/vec2.hpp"
#include <vector>

struct ChunkData
{
	Chunk *chunk = nullptr;
	int used = 0;
	glm::vec2 position = {};
};

static constexpr int chunksSize = 600;

struct ChunkManager
{
	ChunkManager() { loadedChunks.reserve(chunksSize); chunkData.reserve(chunksSize); returnVector.reserve(chunksSize); };

	//todo remove the usage of std::vector
	std::vector<Chunk*> returnVector;
	std::vector<Chunk> loadedChunks;
	std::vector<ChunkData> chunkData;

	//y reserved, should be 0
	Chunk **requestChunks(glm::vec3 *chunks, int size);

	void setupChunk(Chunk *chunk, glm::vec2 p);
	void bakeUnbakedChunks();
	void bakeAllChunks();
};
