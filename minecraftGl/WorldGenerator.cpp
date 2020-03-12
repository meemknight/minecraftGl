#include "WorldGenerator.h"

static FastNoiseSIMD* myNoise = FastNoiseSIMD::NewFastNoiseSIMD();
static FastNoiseSIMD* noiseForBiomes = FastNoiseSIMD::NewFastNoiseSIMD();

void WorldGenerator::setupChunk(Chunk * chunk, glm::vec2 p)
{
	myNoise->SetNoiseType(FastNoiseSIMD::NoiseType::Perlin);
	{
		float scaleXY = 1;
		myNoise->SetAxisScales(1 * scaleXY, 1, 1 * scaleXY);
		myNoise->SetFrequency(0.01);
	}
	
	noiseForBiomes->SetNoiseType(FastNoiseSIMD::NoiseType::Cellular);
	noiseForBiomes->SetCellularReturnType(FastNoiseSIMD::CellularReturnType::NoiseLookup);
	noiseForBiomes->SetCellularDistanceFunction(FastNoiseSIMD::CellularDistanceFunction::Natural);
	noiseForBiomes->SetFrequency(0.02);

	int stonePos;
	int grassPos;

	//todo remove
	//chunk->clearBlockData();

	// Get a set of 16 x 16 x 16 Simplex Fractal noise
	myNoise->SetFractalOctaves(4);
	//float* noiseSet = myNoise->GetSimplexFractalSet(p.x*CHUNK_SIZE, 0, p.y*CHUNK_SIZE, CHUNK_SIZE, BUILD_LIMIT, CHUNK_SIZE, 1);
	int index = 0;

	//float* biomeNoiseData = noiseForBiomes->GetSimplexFractalSet(p.x*CHUNK_SIZE, 0, p.y*CHUNK_SIZE, 1, CHUNK_SIZE, CHUNK_SIZE, 1);

	//stone pass
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			BiomeData biomeData;
			{
				float rx = (x + p.x * CHUNK_SIZE) / biomeNoiseCompression;
				float rz = (z + p.y * CHUNK_SIZE) / biomeNoiseCompression;
				//biomeData = blendBiomes((double)biomeNoiseData[x * CHUNK_SIZE + z]);
				biomeData = blendBiomes(biomeNoise.noise2D_0_1(rx, rz));
			}

			for (int y = 0; y < biomeData.minStonePos; y++)
			{
				chunk->getBlock(x, y, z) = BLOCK::stone;
			}
			 
			{
				float rx = (x + p.x * CHUNK_SIZE) / stoneNoiseCompression;
				float rz = (z + p.y * CHUNK_SIZE) / stoneNoiseCompression;
				for (int y = biomeData.minStonePos; y < biomeData.maxStonePos; y++)
				{
					float ry = y / heightNoiseCompression;
					int delta = biomeData.maxStonePos - biomeData.minStonePos;
					float percent = 1.5 - ((float)(y - biomeData.minStonePos) / float(delta));
					float newStoneCnance = biomeData.stoneChance * std::powf(percent, biomeData.realismExponent);

					float noiseVal = stoneNoise.accumulatedOctaveNoise3D_0_1(rx, ry, rz, biomeData.octaves);

					//float noiseVal = noiseSet[x*CHUNK_SIZE*BUILD_LIMIT + y * CHUNK_SIZE + z];

					if (noiseVal < newStoneCnance)
					{
						chunk->getBlock(x, y, z) = BLOCK::stone;
					}else
					{
						chunk->getBlock(x, y, z) = BLOCK::air;
					}
				}
			}

			for (int y = biomeData.maxStonePos; y < BUILD_LIMIT; y++)
			{
				chunk->getBlock(x, y, z) = BLOCK::air;
			}

			///grass && biome pass
			{
				//todo optimise this line
				float rx = (x + p.x * CHUNK_SIZE) / dirtHeigthCompresion;
				float rz = (z + p.y * CHUNK_SIZE) / dirtHeigthCompresion;
				int dirtDepth = biomeHeigthNoise.noise2D_0_1(rx, rz) * biomeData.bottomBlockDepth;
				for (int y = biomeData.maxStonePos; y >= biomeData.minStonePos - 1; y--)
				{
					if (chunk->getBlock(x, y, z) == BLOCK::stone && chunk->getBlock(x, y + 1, z) == BLOCK::air)
					{
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

	//FastNoiseSIMD::FreeNoiseSet(noiseSet);
	//FastNoiseSIMD::FreeNoiseSet(biomeNoiseData);

}

Block blockTest[] = 
{
	0,0,12,0,
	0,0,12,0,
	0,0,12,0,

	0,0,12,0,
	4,4,4,12,
	0,0,12,0,

	0,0,12,0,
	0,0,12,0,
	0,0,12,0,

};

void generateStructure(ChunkManager &cm, Block *blocks, glm::ivec3 pos, glm::ivec3 size, bool placeOverAnything, bool placeAir)
{
	blocks = blockTest;
	size = { 3,4,3 };

	glm::ivec3 end = pos + size;

	for (int x = pos.x; x < end.x; x++)
		for (int z = pos.z; z < end.z; z++)
			for (int y = pos.y; y < end.y; y++)
			{
				Block b = blocks[y - pos.y + ((z - pos.z) * size.y) + ((x - pos.x)*size.y * size.z)];

				if(placeOverAnything)
				{
					if ((b == BLOCK::air && placeAir) || b != BLOCK::air)
					{

						cm.setBlock({ x,y,z }, b);
					}
				}else
				{
					Block currentBlock = cm.getBlock({ x,y,z });
					if(!isCollideble(currentBlock))
					{
						if ((b == BLOCK::air && placeAir) || b != BLOCK::air)
						{

							cm.setBlock({ x,y,z }, b);
						}
					}
				}

			}


}
