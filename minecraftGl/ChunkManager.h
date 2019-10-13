#pragma once
#include "Chunk.h"
#include "glm/vec2.hpp"
#include <vector>

struct ChunkData
{
	Chunk *chunk = nullptr;
	int used = 0;
	glm::ivec2 position = {};
};

struct ChunkManager
{
private:
	//todo make another struct here
	std::vector<ChunkData> chunksForSort;
public:
	int chunksCount = 0;

	ChunkManager() {};

	//todo remove the usage of std::vector
	std::vector<Chunk*> returnVector;
	std::vector<Chunk> loadedChunks;
	std::vector<ChunkData> chunkData;

	void reserveData(int size);

	//y reserved, should be 0
	Chunk **requestChunks(glm::ivec3 *chunks, int size);

	Chunk **requestChunk(glm::ivec3 chunk);

	Block getBlock(glm::ivec3 pos);
	void setBlock(glm::ivec3 pos, Block b);

	//unsafe because it doesn't update chunks 
	Block &getBlockRefUnsafe(glm::ivec3 pos, Chunk **outC = nullptr);

	void setupChunk(Chunk *chunk, glm::vec2 p);
	void bakeUnbakedChunks(int number);
	void bakeAllChunks();
};
