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
			BiomeData biomeData;
			{
				double rx = (x + p.x * CHUNK_SIZE) / biomeNoiseCompression;
				double rz = (z + p.y * CHUNK_SIZE) / biomeNoiseCompression;
				biomeData = blendBiomes(biomeNoise.noise0_1(rx, rz));
			}

			for (int y = 0; y < biomeData.minStonePos; y++)
			{
				chunk->getBlock(x, y, z) = BLOCK::stone;
			}
			 

			double rx = (x + p.x * CHUNK_SIZE) / stoneNoiseCompression;
			double rz = (z + p.y * CHUNK_SIZE) / stoneNoiseCompression;
			for (int y = biomeData.minStonePos; y < biomeData.maxStonePos; y++)
			{
				double ry = y / heightNoiseCompression;
				int delta = biomeData.maxStonePos - biomeData.minStonePos;
				float percent = 1.5-((float)(y - biomeData.minStonePos)/ float(delta));
				float newStoneCnance = biomeData.stoneChance * std::pow(percent, biomeData.realismExponent);
				
				if(stoneNoise.octaveNoise0_1(rx, ry, rz, biomeData.octaves) < newStoneCnance)
				{
					chunk->getBlock(x, y, z) = BLOCK::stone;
				}
			}

			///grass && biome pass
			//todo optimize
			for (int y = biomeData.maxStonePos; y >= biomeData.minStonePos - 1; y--)
			{
				
				if (chunk->getBlock(x, y, z) == BLOCK::stone && chunk->getBlock(x, y + 1, z) == BLOCK::air)
				{
					double rx = (x + p.x * CHUNK_SIZE) / dirtHeigthCompresion;
					double rz = (z + p.y * CHUNK_SIZE) / dirtHeigthCompresion;

					int dirtDepth = biomeHeigthNoise.noise0_1(rx, rz) * biomeData.bottomBlockDepth;
					chunk->getBlock(x, y, z) = biomeData.topBlock;
					for (int copy = y - 1; copy > y - dirtDepth; copy--)
					{
						chunk->getBlock(x, copy, z) = biomeData.bottomBlock;
					}
					y -= dirtDepth + 1;
				}
			}

		}
	}

}
