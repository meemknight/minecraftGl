#include <cmath>
#include <vector>
#include <algorithm>

#include "ChunkManager.h"
#include "PerlinNoise.hpp"
#include "tools.h"
#include "fileHandler.h"
#include "WorldGenerator.h"

extern ChunkFileHandler fileHandler;

WorldGenerator worldGeneraor(1259);

bool lazySave = false;

void ChunkManager::reserveDataAndClear(int size)
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

	cachedChunk = {};
}

void ChunkManager::reserveDataAndNoCleanup(int size)
{
	chunksCount = size;

	returnVector.reserve(chunksCount);

	loadedChunks.resize(chunksCount);
	chunkData.resize(chunksCount);

}

Chunk **ChunkManager::requestChunks(glm::ivec3 *requestedC, int size, bool generateStructures, const glm::ivec2 playerPos)
{
	cachedChunk = {};
	//reset data return data
	returnVector.clear();

	int pos = 0; //pos in requestedC
	bool unallocatedData = 0;

	if(size > chunksCount) //this is if you request more chunks than reserved initially
	{
		ilog("Making the chunk buffer bigger:", size);

		//old version, recreate data
		//first save old data:
		for(int i=0; i< chunksCount; i++)
		{
			if (chunkData[i].chunk->shouldReSave)
			{
				fileHandler.saveChunk(*chunkData[i].chunk);
			}
		}
		
		reserveDataAndClear(size);
		
		//alocate new data
		for (int pos=0; pos < size; pos++)
		{
			//if (requestedC[pos].y == 0)
			{
				//loadedChunks.push_back(Chunk());
				ChunkData cd;
				cd.used = 1;
				cd.position.x = requestedC[pos].x;
				cd.position.y = requestedC[pos].z;
				cd.chunk = &loadedChunks[pos];
		
				setupChunk(&loadedChunks[pos], cd.position, false);
		
				chunkData[pos] = cd;
		
				returnVector.push_back(chunkData[pos].chunk);
			}
		}
		goto endOfFunction;

		//new version, make the buffer bigger todo (all chunks should be valid)
		//reserveDataAndNoCleanup(size);


	}


	/// normal routine
	for (int i = 0; i < chunkData.size(); i++)
	{
		chunkData[i].used = 0;
	}

	//first see what data is used
	for (int i = 0; i < chunkData.size(); i++)
	{
		//this are the requested chunks (0 - size)
		for(int j=0; j<size; j++)
		{
			if (chunkData[i].chunk && chunkData[i].position.x == requestedC[j].x && chunkData[i].position.y == requestedC[j].z)
			{
				chunkData[i].used = 1;
				requestedC[j].y = 1;
				returnVector.push_back(chunkData[i].chunk);
				break;
			}
		}
	}

	//load the other data in unused spaces
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
		auto newPlayerPos = playerPos / 16;
		chunksForSort.clear();
		for (int i = 0; i < chunkData.size(); i++)
		{
			if (chunkData[i].used == 0)
			{
				ChunkDataSimple data;
				data.chunkDataUnion = &chunkData[i];
				data.position = { chunkData[i].position.x, chunkData[i].position.y };
				chunksForSort.push_back(data);
			}
		}

		std::sort(chunksForSort.begin(), chunksForSort.end(), [newPlayerPos](const ChunkDataSimple &first, const ChunkDataSimple &second)
		{
				auto a = first.position - glm::ivec2(newPlayerPos.x, newPlayerPos.y);
				auto b = second.position - glm::ivec2(newPlayerPos.x, newPlayerPos.y);
				return a.x * a.x + a.y * a.y >
					b.x *b.x + b.y * b.y;
		});

		//replce random chunks version
		//for (int i = 0; i < chunkData.size(); i++)
		//{
		//	if (chunkData[i].chunk && chunkData[i].used == 0)
		//	{
		//		chunkData[i].used = 1;
		//		chunkData[i].position.x = requestedC[pos].x;
		//		chunkData[i].position.y = requestedC[pos].z;
		//
		//		setupChunk(chunkData[i].chunk, chunkData[i].position, false);
		//		
		//		returnVector.push_back(chunkData[i].chunk);
		//
		//		unallocatedData = 0;
		//		for (pos++; pos < size; pos++)
		//		{
		//			if (requestedC[pos].y == 0)
		//			{
		//				unallocatedData = 1;
		//				break;
		//			}
		//		}
		//
		//		if(unallocatedData == 0)
		//		{
		//			break;
		//		}
		//	}
		//}

		//replace far chunks version
		for (int i = 0; i < chunksForSort.size(); i++)
		{
			auto &chunkDataElement = *chunksForSort[i].chunkDataUnion;
			if (chunkDataElement.chunk && chunkDataElement.used == 0)
			{
				chunkDataElement.used = 1;
				chunkDataElement.position.x = requestedC[pos].x;
				chunkDataElement.position.y = requestedC[pos].z;
		
				setupChunk(chunkDataElement.chunk, chunkDataElement.position, false);
				
				returnVector.push_back(chunkDataElement.chunk);
		
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
	
	endOfFunction:

	//here load trees
	if(generateStructures)
	for(auto &c: returnVector)
	{
		glm::ivec3 pos = c->position;
		unsigned long distance =
			(pos.x * CHUNK_SIZE - playerPos.x) * (pos.x* CHUNK_SIZE - playerPos.x) + (pos.z* CHUNK_SIZE - playerPos.y) * (pos.z* CHUNK_SIZE - playerPos.y);

		//todo change here based on distance
		if(distance < 6'400)
		{
			worldGeneraor.setupStructuresInChunk(c, {c->position.x, c->position.z}, *this);
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

	auto c = (Chunk**)requestChunks(&chunk, 1, 0, {});

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

	c->shouldRecreate = true;
	c->shouldReSave = true;

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

void ChunkManager::setupChunk(Chunk *chunk, glm::vec2 p, bool buildChunk)
{
	if(chunk->shouldReSave)
	{
		fileHandler.saveChunk(*chunk);
	}

	chunk->shouldReSave = false;
	chunk->removeNeighboursLinkage();
	//chunk->clearBlockData();
	chunk->resetMeshes();
	chunk->position = { p.x, 0 ,p.y };

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

	//chunk->bakeMeshes();
	chunk->shouldRecreate = true;
	chunk->resetMeshes();
	//for (int i = 0; i < FACE::FACES_SIZE; i++)
	//{
	//	chunk->positionData[i].size = 0;
	//}

	

	if(buildChunk)
	{
		chunk->chunkBuilt = 1;
		//let this be the last instruction for cache considerations
		if (!fileHandler.loadChunk(*chunk))
		{
			chunk->shouldReSave = !lazySave;//if this is true, chunks are always saved
			worldGeneraor.setupChunk(chunk, p);
		}
	}else
	{
		chunk->chunkBuilt = 0;
	}

}

//todo basic optimise
//todo algoritmically optimise
//todo fix it (request small numbers)
//bake means compute for gpu
void ChunkManager::bakeUnbakedChunks(int numberToBake, int numberToBuild, glm::vec2 pos)
{
	pos /= 16;
	chunksForSort.clear();
	for(int i=0; i< loadedChunks.size(); i++)
	{
		if(loadedChunks[i].shouldRecreate || !loadedChunks[i].chunkBuilt)
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

	int nrTobaleSoFar = 0;
	int nrToBuildSoFar = 0;
	for (auto &i : chunksForSort)
	{
			//todo move this in a function 
			if(nrToBuildSoFar < numberToBuild && !i.chunk->chunkBuilt)
			{
				if (!fileHandler.loadChunk(*i.chunk))
				{
					i.chunk->shouldReSave = !lazySave;//if this is true, chunks are always saved
					worldGeneraor.setupChunk(i.chunk, { i.chunk->position.x, i.chunk->position.z});
				}

				nrToBuildSoFar++;
			}

			if(nrTobaleSoFar< numberToBake)
			{
				if(
					  (i.chunk->neighbours[0] != nullptr && i.chunk->neighbours[0]->chunkBuilt == false)
					||(i.chunk->neighbours[1] != nullptr && i.chunk->neighbours[1]->chunkBuilt == false)
					||(i.chunk->neighbours[2] != nullptr && i.chunk->neighbours[2]->chunkBuilt == false)
					||(i.chunk->neighbours[3] != nullptr && i.chunk->neighbours[3]->chunkBuilt == false)
					||(i.chunk->chunkBuilt == false)
					)
				{
				
					//not bake chunks if the chunks near are not made

				}else
				{

					i.chunk->shouldRecreate = false;
					i.chunk->bakeMeshes();
					nrTobaleSoFar++;
				}

			}
	

			if(nrTobaleSoFar >= numberToBake && nrToBuildSoFar >= numberToBuild)
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
