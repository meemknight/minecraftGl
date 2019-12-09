#include <cmath>
#include <vector>
#include <algorithm>

#include "ChunkManager.h"
#include "PerlinNoise.hpp"
#include "tools.h"

static siv::PerlinNoise noise;

float constexpr E = 2.71828182846;

float sigmoid(float x)
{
	return 1.f / (1.f + pow(E, -x));
}

float xEcuation(int pos)
{
	float x = pos / 100.f;
	//x *= (pos / 8) % 2 == 0 ? 1 : -1;
	float rez = sin(x);
	rez = (rez + 1.f) / 2.f;
	//rez = sigmoid(rez);
	rez *= 25;
	return rez;
}

float zEcuation(int pos)
{
	float x = pos / 100.f;
	//x *= (pos / 8) % 2 == 0 ? 1 : -1;
	float rez = cos(x);
	rez = (rez + 1.f) / 2.f;
	//rez = sigmoid(rez);
	rez *= 25;
	return rez;
}

int xzEcuatiom(int x, int z)
{
	return (int)((xEcuation(x) + zEcuation(z)) / 2.f);
}

void ChunkManager::reserveData(int size)
{
	chunksCount = size;
	loadedChunks.clear();
	chunkData.clear();
	returnVector.clear();
	
	loadedChunks.reserve(chunksCount); 
	chunkData.reserve(chunksCount); 
	returnVector.reserve(chunksCount);

	loadedChunks.resize(chunksCount);
	chunkData.resize(chunksCount);

}

Chunk **ChunkManager::requestChunks(glm::ivec3 *requestedC, int size)
{
	cachedChunk = {};
	//reset data return data
	returnVector.clear();

	if(size > chunksCount)
	{
		reserveData(size);

		//alocate new data
		for (int pos=0; pos < size; pos++)
		{
			if (requestedC[pos].y == 0)
			{
				//loadedChunks.push_back(Chunk());
				ChunkData cd;
				cd.used = 1;
				cd.position.x = requestedC[pos].x;
				cd.position.y = requestedC[pos].z;
				cd.chunk = &loadedChunks[pos];

				setupChunk(&loadedChunks[pos], cd.position);

				chunkData[pos] = cd;

				returnVector.push_back(chunkData[pos].chunk);
			}
		}

		return returnVector.data();
	}

	for (int i = 0; i < chunkData.size(); i++)
	{
		chunkData[i].used = 0;
	}

	//first see what data is used
	for (int i = 0; i < chunkData.size(); i++)
	{
		for(int j=0; j<size; j++)
		{
			if (chunkData[i].chunk && chunkData[i].position.x == requestedC[j].x && chunkData[i].position.y == requestedC[j].z)
			{
				chunkData[i].used = 1;
				requestedC[j].y = 1;
				returnVector.push_back(chunkData[i].chunk);
			}
		}
	}

	//load the other data in unused spaces
	int pos = 0; //pos in requestedC
	bool unallocatedData = 0;

	for(pos=0; pos<size;pos++)
	{
		if(requestedC[pos].y == 0)
		{
			unallocatedData = 1;
			break;
		}
	}

	if(unallocatedData)
	{
		for (int i = 0; i < chunkData.size(); i++)
		{
			if (chunkData[i].chunk && chunkData[i].used == 0)
			{
				chunkData[i].used = 1;
				chunkData[i].position.x = requestedC[pos].x;
				chunkData[i].position.y = requestedC[pos].z;

				setupChunk(chunkData[i].chunk, chunkData[i].position);
				
				returnVector.push_back(chunkData[i].chunk);

				unallocatedData = 0;
				for (pos++; pos < size; pos++)
				{
					if (requestedC[pos].y == 0)
					{
						unallocatedData = 1;
						break;
					}
				}

				if(unallocatedData == 0)
				{
					break;
				}
			}
		}

	}
	
	
	return returnVector.data();
}

Chunk **ChunkManager::requestChunk(glm::ivec3 chunk)
{
	if(cachedChunk.chunk&&chunk.x==cachedChunk.position.x && chunk.z == cachedChunk.position.y)
	{
		return &cachedChunk.chunk;
	}

	auto c = (Chunk**)requestChunks(&chunk, 1);

	cachedChunk.chunk = *c;
	cachedChunk.position.x = chunk.x;
	cachedChunk.position.y = chunk.z;

	return c;
}

Block &ChunkManager::getBlockRefUnsafe(glm::ivec3 pos, Chunk **outC)
{
	glm::ivec3 chunkPos = pos / CHUNK_SIZE; 
	chunkPos.y = 0;
	if (pos.x < 0) { chunkPos.x--;}
	if (pos.z < 0) { chunkPos.z--;}
	
	pos -= chunkPos * CHUNK_SIZE;

	if (pos.x >= 16) { pos.x = 0; chunkPos.x++; }
	if (pos.z >= 16) { pos.z = 0; chunkPos.z++;}


	Chunk **c = requestChunk(chunkPos);

	//ilog(c[0]->position.x, c[0]->position.z);

	//todo when i perform collision on blocks outside, there should be something here to handle this prooblem.
	Block &b =c[0]->getBlock(pos);
	c[0]->shouldRecreate = 1;

	if (outC)
	{
		*outC = c[0];
	}

	return b;
}

Block ChunkManager::getBlock(glm::ivec3 pos)
{
	if(pos.y<0 || pos.y >= BUILD_LIMIT)
	{
		return BLOCK::air;
	}else
	{
		return getBlockRefUnsafe(pos);
	}
}

void ChunkManager::setBlock(glm::ivec3 pos, Block b)
{
	if (pos.y < 0 || pos.y >= BUILD_LIMIT)
	{
		return;
	}

	Chunk *c;
	getBlockRefUnsafe(pos, &c) = b;

	if(pos.x >= 0 && pos.x % 16 == 15)
	{
		if (c->neighbours[CN::right])
			c->neighbours[CN::right]->shouldRecreate = 1;
	}else if(-pos.x % 16 == 1)
	{
		if (c->neighbours[CN::right])
			c->neighbours[CN::right]->shouldRecreate = 1;
	}

	if(pos.x % 16 == 0)
	{
		if (c->neighbours[CN::left])
			c->neighbours[CN::left]->shouldRecreate = 1;
	}

	if (pos.z >= 0 && pos.z % 16 == 15)
	{
		if (c->neighbours[CN::front])
			c->neighbours[CN::front]->shouldRecreate = 1;
	}
	else if (-pos.z % 16 == 1)
	{
		if (c->neighbours[CN::front])
			c->neighbours[CN::front]->shouldRecreate = 1;
	}

	if (pos.z % 16 == 0)
	{
		if (c->neighbours[CN::back])
			c->neighbours[CN::back]->shouldRecreate = 1;
	}

}

void ChunkManager::setupChunk(Chunk *chunk, glm::vec2 p)
{
	chunk->removeNeighboursLinkage();
	chunk->clear();
	chunk->resetMeshes();
	chunk->position = { p.x, 0 ,p.y };
	int posX = p.x;
	int posZ = p.y;
#pragma region neighbours

	Chunk *neighbours[4] = {};

	glm::ivec2 positions[4];
	positions[CN::front] = { p.x, p.y + 1 };
	positions[CN::back] = { p.x, p.y - 1 };
	positions[CN::left] = { p.x - 1, p.y };
	positions[CN::right] = { p.x + 1, p.y };

	int count = 0;

	for (int i = 0; i < chunkData.size(); i++)
	{
		for (int dir = 0; dir < 4; dir++)
		{
			if (chunkData[i].chunk && positions[dir] == chunkData[i].position)
			{
				neighbours[dir] = chunkData[i].chunk;
				count++;
				if (count == 4)
				{
					goto foundAll;
				}
			}
		}
	}

foundAll:

	if (neighbours[CN::front])
	{
		neighbours[CN::front]->neighbours[CN::back] = chunk;
		neighbours[CN::front]->shouldRecreate = true;
	}

	if (neighbours[CN::back])
	{
		neighbours[CN::back]->neighbours[CN::front] = chunk;
		neighbours[CN::back]->shouldRecreate = true;
	}

	if (neighbours[CN::left])
	{
		neighbours[CN::left]->neighbours[CN::right] = chunk;
		neighbours[CN::left]->shouldRecreate = true;
	}

	if (neighbours[CN::right])
	{
		neighbours[CN::right]->neighbours[CN::left] = chunk;
		neighbours[CN::right]->shouldRecreate = true;
	}

	for (int i = 0; i < 4; i++)
	{
		chunk->neighbours[i] = neighbours[i];
	}

#pragma endregion

	int stonePos;
	int grassPos;

		/*
		for(int x=0; x<16; x++)
		{
			for(int z=0; z<16; z++)
			{
				stonePos = mountains[(x + (posX * 16) + (tx * (z + (posZ * 16))))];
				stonePos = min(stonePos, 250);
				grassPos = stonePos + 5;
	
				for(int y=0; y<stonePos; y++)
				{
					chunk->getBlock(x, y, z) = BLOCK::stone;
				}

				for (int y = stonePos; y < grassPos-1; y++)
				{
					chunk->getBlock(x, y, z) = BLOCK::dirt;
				}

				chunk->getBlock(x, grassPos-1, z) = BLOCK::grass;

			}
			
		}
		*/
	/*
	for (int x = 0; x < 16; x++)
	{
		for (int z = 0; z < 16; z++)
		{
			for (int y = 0; y < 30; y++)
			{
				if (y < 5)
				{
					chunk->getBlock(x, y, z) = BLOCK::stone;
				}
				else
				{
					chunk->getBlock(x, y, z) = (x + z) / 2 < y ? BLOCK::grass : BLOCK::dirt;
					if ((x + z) / 2 < y)
					{
						break;
					}
				}

			}
		}
	}
	*/
	
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			double rx = (x + p.x * CHUNK_SIZE) / 160.0;
			double rz = (z + p.y * CHUNK_SIZE) / 160.0;

			stonePos = 150 * noise.octaveNoise0_1(rx, rz, 8);
			grassPos = stonePos + 2;
		
			for (int y = 0; y < stonePos; y++)
			{
				chunk->getBlock(x, y, z) = BLOCK::stone;
			}

			for (int y = stonePos; y < grassPos - 1; y++)
			{
				chunk->getBlock(x, y, z) = BLOCK::dirt;
			}

			chunk->getBlock(x, grassPos - 1, z) = BLOCK::grass;
		}
	}
	
	//chunk->bakeMeshes();
	chunk->shouldRecreate = true;
	for (int i = 0; i < FACE::FACES_SIZE; i++)
	{
		chunk->positionData[i].size = 0;
	}

}

//todo basic optimise
//todo algoritmically optimise
//todo fix it (request small numbers)
void ChunkManager::bakeUnbakedChunks(int number, glm::vec2 pos)
{
	pos /= 16;
	chunksForSort.clear();
	for(int i=0; i< loadedChunks.size(); i++)
	{
		if(loadedChunks[i].shouldRecreate)
		{
			chunksForSort.push_back({ &loadedChunks[i], {loadedChunks[i].position.x, loadedChunks[i].position.z} });
		}	
	}

	std::sort(chunksForSort.begin(), chunksForSort.end(), [pos](const ChunkDataSimple &first, const ChunkDataSimple &second)
	{
		auto a = first.position - glm::ivec2(pos.x, pos.y);
		auto b = second.position - glm::ivec2(pos.x, pos.y);
		return a.x * a.x + a.y * a.y <
			b.x * b.x + b.y * b.y;
	});

	int soFar = 0;
	for (auto &i : chunksForSort)
	{
			i.chunk->shouldRecreate = false;
			i.chunk->bakeMeshes();
			soFar++;
			if(soFar >= number)
			{
				break;
			}	
	}

}

void ChunkManager::bakeAllChunks()
{
	for (auto &i : loadedChunks)
	{
		i.bakeMeshes();
		i.shouldRecreate = false;
	}
}
