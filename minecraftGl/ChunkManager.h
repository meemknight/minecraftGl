#pragma once
#include "Chunk.h"
#include "pinnedVector.h"
#include "glm/vec2.hpp"
#include <vector>

struct ChunkData
{
	Chunk *chunk = nullptr;
	int used = 0;
	glm::ivec2 position = {};
};

struct ChunkDataSimple
{
	union
	{
		Chunk *chunk = nullptr;
		ChunkData *chunkDataUnion;
	};
	glm::ivec2 position = {};
};

struct ChunkManager
{
private:
	PinnedVector<ChunkDataSimple> chunksForSort;
	ChunkDataSimple cachedChunk;
public:
	//todo remove this
	int chunksCount = 0;

	ChunkManager() {};

	PinnedVector<Chunk*> returnVector;
	PinnedVector<Chunk> loadedChunks;
	PinnedVector<ChunkData> chunkData;

	void reserveDataAndClear(int size);

	void reserveDataAndNoCleanup(int size);

	//y reserved, should be 0
	Chunk **requestChunks(glm::ivec3 *chunks, int size, bool generateStructures, const glm::ivec2 playerPos);

	Chunk **requestChunk(glm::ivec3 chunk);

	Block getBlock(glm::ivec3 pos);
	void setBlock(glm::ivec3 pos, Block b);

	//unsafe because it doesn't update chunks 
	Block &getBlockRefUnsafe(glm::ivec3 pos, Chunk **outC = nullptr);

	void setupChunk(Chunk *chunk, glm::vec2 p, bool buildChunk);
	void bakeUnbakedChunks(int numberToBake, int numberToBuild, glm::vec2 pos= {});
	void bakeAllChunks();
};
