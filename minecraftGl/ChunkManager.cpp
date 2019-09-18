#include "ChunkManager.h"
#include <cmath>
#include "PerlinNoise.hpp"

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

Chunk **ChunkManager::requestChunks(glm::vec3 *requestedC, int size)
{
	//reset data
	returnVector.clear();

	for (int i = 0; i < chunkData.size(); i++)
	{
		chunkData[i].used = 0;
	}

	//first see what data is used
	for (int i = 0; i < chunkData.size(); i++)
	{
		for(int j=0; j<size; j++)
		{
			if (chunkData[i].position.x == requestedC[j].x && chunkData[i].position.y == requestedC[j].z)
			{
				chunkData[i].used = 1;
				requestedC[j].y = 1.f;
				returnVector.push_back(chunkData[i].chunk);
			}
		}
	}

	//load the other data in unused spaces
	int pos = 0; //pos in requestedC
	bool unallocatedData = 0;

	for(pos=0; pos<size;pos++)
	{
		if(requestedC[pos].y == 0.f)
		{
			unallocatedData = 1;
			break;
		}
	}

	if(unallocatedData)
	{
		for (int i = 0; i < chunkData.size(); i++)
		{
			if (chunkData[i].used == 0)
			{
				chunkData[i].used = 1;
				chunkData[i].position.x = requestedC[pos].x;
				chunkData[i].position.y = requestedC[pos].z;

				setupChunk(chunkData[i].chunk, chunkData[i].position);
				
				returnVector.push_back(chunkData[i].chunk);

				unallocatedData = 0;
				for (pos++; pos < size; pos++)
				{
					if (requestedC[pos].y == 0.f)
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
	
	//alocate new data
	for (; pos < size; pos++)
	{
		if (requestedC[pos].y == 0.f)
		{
			loadedChunks.push_back(Chunk());
			ChunkData cd;
			cd.used = 1;
			cd.position.x = requestedC[pos].x;
			cd.position.y = requestedC[pos].z;
			cd.chunk = &loadedChunks[loadedChunks.size() - 1];

			setupChunk(&loadedChunks[loadedChunks.size()-1], cd.position);

			chunkData.emplace_back(cd);

			returnVector.push_back(chunkData[chunkData.size()-1].chunk);
		}
	}
	
	return returnVector.data();
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

	glm::vec2 positions[4];
	positions[CN::front] = { p.x, p.y + 1 };
	positions[CN::back] = { p.x, p.y - 1 };
	positions[CN::left] = { p.x - 1, p.y };
	positions[CN::right] = { p.x + 1, p.y };

	int count = 0;

	for (int i = 0; i < chunkData.size(); i++)
	{
		for (int dir = 0; dir < 4; dir++)
		{
			if (positions[dir] == chunkData[i].position)
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
}

void ChunkManager::bakeUnbakedChunks()
{
	for (auto &i : loadedChunks)
	{
		if (i.shouldRecreate)
		{
			i.bakeMeshes();
			i.bakeMeshes();
		}
		i.shouldRecreate = false;
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
