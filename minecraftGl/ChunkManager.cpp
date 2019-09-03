#include "ChunkManager.h"

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
			if (chunkData[i].used == 0)
			{
				chunkData[i].used = 1;
				chunkData[i].position.x = requestedC[pos].x;
				chunkData[i].position.y = requestedC[pos].z;

				returnVector.push_back(chunkData[i].chunk);

				unallocatedData = 0;
				for (; pos < size; pos++)
				{
					if (requestedC[pos].y == 0)
					{
						unallocatedData = 1;
						break;
					}
				}

				setupChunk(chunkData[i].chunk, chunkData[i].position);

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
		if (requestedC[pos].y == 0)
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
	chunk->position = { p.x, 0 ,p.y };

	Chunk *neighbours[4] = {};

	glm::vec2 positions[4];
	positions[CN::front] = { p.x, p.y + 1 };
	positions[CN::back] = { p.x, p.y - 1 };
	positions[CN::left] = { p.x - 1, p.y };
	positions[CN::right] = { p.x + 1, p.y };

	int count = 0;

	for(int i=0; i<chunkData.size(); i++)
	{
		for(int dir = 0; dir<4; dir++)
		{
			if (positions[dir]==chunkData[i].position)
			{
				neighbours[dir] = chunkData[i].chunk;
				count++;
				if(count == 4)
				{
					goto foundAll;
				}
			}
		}
	}

	foundAll:

	if(neighbours[CN::front])
	{
		neighbours[CN::front]->neighbours[CN::back] = chunk;
	}

	if (neighbours[CN::back])
	{
		neighbours[CN::back]->neighbours[CN::front] = chunk;
	}

	if (neighbours[CN::left])
	{
		neighbours[CN::left]->neighbours[CN::right] = chunk;
	}

	if (neighbours[CN::right])
	{
		neighbours[CN::right]->neighbours[CN::left] = chunk;
	}

	for(int i=0;i<4;i++)
	{
		chunk->neighbours[i] = neighbours[i];
	}

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

}