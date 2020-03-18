#include "WorldGenerator.h"
#include "tools.h"

static FastNoiseSIMD* myNoise = FastNoiseSIMD::NewFastNoiseSIMD();
static FastNoiseSIMD* noiseForBiomes = FastNoiseSIMD::NewFastNoiseSIMD();

Block tree1[] =
{
	0,0,0,12,0,
	0,0,0,12,0,
	0,0,0,12,0,

	0,0,0,12,0,
	4,4,4,4,12,
	0,0,0,12,0,

	0,0,0,12,0,
	0,0,0,12,0,
	0,0,0,12,0,

};


std::mt19937_64 randNumberGen;

float WorldGenerator::mt_0_1(std::uint64_t seed)
{
	std::uniform_real_distribution<float> dist(0.f, 1.f);
	randNumberGen.seed(seed);
	return dist(randNumberGen);
}

void WorldGenerator::setupChunk(Chunk * chunk, glm::vec2 p)
{
	myNoise->SetNoiseType(FastNoiseSIMD::NoiseType::Perlin);
	{
		float scale = 1.5;
		myNoise->SetAxisScales(scale, scale, scale);
		//note: small frequency is good for ores and things like that
		myNoise->SetFrequency(0.022);
		myNoise->SetFractalOctaves(2);

	}
	
	//noiseForBiomes->SetNoiseType(FastNoiseSIMD::NoiseType::Cellular);
	//noiseForBiomes->SetCellularReturnType(FastNoiseSIMD::CellularReturnType::NoiseLookup);
	//noiseForBiomes->SetCellularDistanceFunction(FastNoiseSIMD::CellularDistanceFunction::Natural);
	//noiseForBiomes->SetFrequency(0.02);

	int stonePos;
	int grassPos;


	//Get a set of 16 x 16 x 16 Simplex Fractal noise
	float* caveNoiseSet = myNoise->GetSimplexFractalSet(p.x*CHUNK_SIZE, 0, p.y*CHUNK_SIZE, CHUNK_SIZE, (BUILD_LIMIT-10), CHUNK_SIZE, 1);
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

			///caves pass
			int maxH = std::min(BUILD_LIMIT - 10, (int)biomeData.maxStonePos);
			for (int y = 1; y < maxH; y++)
			{
				float caveNoiseVal = caveNoiseSet[x*CHUNK_SIZE*(BUILD_LIMIT - 10) + y * CHUNK_SIZE + z];
				float caveTressholdSets[] = { 0.5,0.3, 0.32, 0.42, 0.5, 0.6,1 };
				float caveTressHold = caveTressholdSets
				[(int)(((float)y / (float)maxH) * (sizeof(caveTressholdSets) / sizeof(caveTressholdSets[0])))];

				//small numbers are more lickely
				if (caveNoiseVal > caveTressHold)
				{
					chunk->getBlock(x, y, z) = BLOCK::air;
				}

			}

		}
	}

	FastNoiseSIMD::FreeNoiseSet(caveNoiseSet);
	//FastNoiseSIMD::FreeNoiseSet(biomeNoiseData);

}

//the position is rel to chunk mesh
void WorldGenerator::setupStructuresInChunk(Chunk * chunk, glm::vec2 p, ChunkManager &cm)
{
	if(chunk->fullyLoaded)
	{
		return;
	}

	chunk->fullyLoaded = 1;

	float *noiseVal = noiseForTrees->GetWhiteNoiseSet(p.x*CHUNK_SIZE, 0, p.y*CHUNK_SIZE, CHUNK_SIZE, 1, CHUNK_SIZE, 10);

	for(int x=0; x<CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			float rx = (x + p.x * CHUNK_SIZE) / treeNoiseCompression;
			float rz = (z + p.y * CHUNK_SIZE) / treeNoiseCompression;

			float val = treeNoise.noise2D_0_1(rx, rz);

			//ilog(val);

			//this determins the freqvency of forests
			if(val > 0.6)
			{

				int xPos = x + p.x * CHUNK_SIZE;
				int zPos = z + p.y * CHUNK_SIZE;
				int yPos = 0;

				if (noiseVal[x + CHUNK_SIZE * z] > 0.85f)
				{
					for (int y = BUILD_LIMIT - 50; y > 100; y--)
					{
						auto b = (chunk->getBlock({ x, y, z }));
						if (b == BLOCK::grass || b == BLOCK::snow_dirt)
						{
							yPos = y + 1;
							break;
						}
					}

					if (yPos)
					{
						cm.setBlock({ xPos, yPos - 1, zPos }, BLOCK::dirt);
						generateStructure(cm, tree1, { xPos - 1 , yPos, zPos - 1 }, { 3,5,3 }, 1, 0);
					}

					//if(x == 15)
				}

			}

		}

	}

	FastNoiseSIMD::FreeNoiseSet(noiseVal);

}

void generateStructure(ChunkManager &cm, Block *blocks, glm::ivec3 pos, glm::ivec3 size, bool placeOverAnything, bool placeAir)
{

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
