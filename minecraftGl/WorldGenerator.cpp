#include "WorldGenerator.h"

void WorldGenerator::setupChunk(Chunk * chunk, glm::vec2 p)
{
	int stonePos;
	int grassPos;

	//todo remove
	chunk->clearBlockData();

	//stone pass
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			for (int y = 0; y < minStonePos; y++)
			{
				chunk->getBlock(x, y, z) = BLOCK::stone;
			}

			double rx = (x + p.x * CHUNK_SIZE) / stoneNoiseCompression;
			double rz = (z + p.y * CHUNK_SIZE) / stoneNoiseCompression;
			for (int y = minStonePos; y < maxStonePos; y++)
			{
				double ry = y / heightNoiseCompression;
				int delta = maxStonePos - minStonePos;
				float percent = 1.5-((float)(y - minStonePos)/ float(delta));
				float newStoneCnance = stoneChance * percent * percent;

				if(stoneNoise.octaveNoise0_1(rx, ry, rz, stoneOctaves) < newStoneCnance)
				{
					chunk->getBlock(x, y, z) = BLOCK::stone;
				}
			}

			///grass && biome pass
			//todo optimize
			for (int y = maxStonePos; y > minStonePos; y--)
			{
				
				if (chunk->getBlock(x, y, z) == BLOCK::stone && chunk->getBlock(x, y + 1, z) == BLOCK::air)
				{
					double rx = (x + p.x * CHUNK_SIZE) / dirtHeigthCompresion;
					double rz = (z + p.y * CHUNK_SIZE) / dirtHeigthCompresion;

					int dirtDepth = biomeHeigthNoise.noise0_1(rx, rz) * dirtHeigth;
					chunk->getBlock(x, y, z) = BLOCK::grass;
					for (int copy = y - 1; copy > y - dirtDepth; copy--)
					{
						chunk->getBlock(x, copy, z) = BLOCK::dirt;
					}
					y -= dirtDepth + 1;
				}
			}

		}
	}


	/*
		for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			for (int y = minStonePos; y < maxStonePos; y++)
			{
				double rx = (x + p.x * CHUNK_SIZE) / stoneNoiseCompression;
				double rz = (z + p.y * CHUNK_SIZE) / stoneNoiseCompression;
				
				int stonePos = ((maxStonePos - minStonePos) * stoneNoise.octaveNoise0_1(rx, rz, 8)) + minStonePos;

				for (int y = 0; y < stonePos; y++)
				{
					chunk->getBlock(x, y, z) = BLOCK::stone;
				}
			}
		}
	}
	*/

	/*
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			double rx = (x + p.x * CHUNK_SIZE) / 160.0;
			double rz = (z + p.y * CHUNK_SIZE) / 160.0;

			stonePos = 150 * stoneNoise.octaveNoise0_1(rx, rz, 8);
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

			for (int y = grassPos; y < BUILD_LIMIT; y++)
			{
				chunk->getBlock(x, y, z) = BLOCK::air;
			}
		}
	}
	*/
}
