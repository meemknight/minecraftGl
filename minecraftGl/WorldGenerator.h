#pragma once
#include "PerlinNoise.hpp"
#include "Chunk.h"
#include <glm/vec2.hpp>
#include "Blocks.h"

struct WorldGenerator
{
	struct BiomeData;
private:
	std::vector<BiomeData> biomes;
	int chanceCounter = 0;
public:

	bool blendUp = true;
	bool biomeBlend = true;

	struct BiomeData
	{
		int chanceFactor;
		Block topBlock;
		Block bottomBlock;
		float bottomBlockDepth;
		float minStonePos;
		float maxStonePos;
	};

	void setDefaultBiomes()
	{
		biomes.resize(3);

		//desert
		biomes[0].chanceFactor = 1;
		biomes[0].topBlock = BLOCK::sand;
		biomes[0].bottomBlock = BLOCK::sand_stone;
		biomes[0].bottomBlockDepth = 10;
		biomes[0].minStonePos = 80;
		biomes[0].maxStonePos = 100;

		//plains
		biomes[1].chanceFactor = 2;
		biomes[1].topBlock = BLOCK::grass;
		biomes[1].bottomBlock = BLOCK::dirt;
		biomes[1].bottomBlockDepth = 6;
		biomes[1].minStonePos = 90;
		biomes[1].maxStonePos = 150;

		//ice
		biomes[2].chanceFactor = 1;
		biomes[2].topBlock = BLOCK::snow_dirt;
		biomes[2].bottomBlock = BLOCK::dirt;
		biomes[2].bottomBlockDepth = 4;
		biomes[2].minStonePos = 120;
		biomes[2].maxStonePos = 200;

		for(const auto &i: biomes)
		{
			chanceCounter += i.chanceFactor;
		}
	}

	int getBiomeIndex(double perc)
	{
		perc *= chanceCounter;
		
		for(int i=0; i<biomes.size(); i++)
		{
			perc -= biomes[i].chanceFactor;
			if(perc <= 0)
			{
				return i;
			}
		}
	}

	BiomeData blendBiomes(double perc)
	{
		float calcPerc = perc * chanceCounter;
		float advance = perc * chanceCounter;
		int pos = 0;

		int accumulated = 0;

		for (int i = 0; i < biomes.size(); i++)
		{
			calcPerc -= biomes[i].chanceFactor;
			if (calcPerc <= 0)
			{
				pos = i;
				break;
			}
			accumulated += biomes[i].chanceFactor;
		}
		
		if(!biomeBlend)
		{
			return biomes[pos];
		}

		//see where is advance:
		float percentIn = (advance - accumulated) / (float)biomes[pos].chanceFactor;
		
		if(percentIn >= 0.8 && pos < biomes.size()-1 && blendUp)//blend right
		{
			BiomeData bd = biomes[pos];
			const BiomeData &bd2 = biomes[pos + 1];
			float oneMinusPercentKeep = (percentIn - 0.8) / 0.2;
			float percentKeep = 1-oneMinusPercentKeep;

			bd.bottomBlockDepth = bd.bottomBlockDepth * percentKeep + bd2.bottomBlockDepth * oneMinusPercentKeep;
			bd.bottomBlockDepth = bd.bottomBlockDepth < 1 ? 1 : bd.bottomBlockDepth;
			bd.minStonePos = bd.minStonePos * percentKeep + bd2.minStonePos * oneMinusPercentKeep;
			bd.maxStonePos = bd.maxStonePos * percentKeep + bd2.maxStonePos * oneMinusPercentKeep;
			
			//blend up
			return bd;
		}else
		if (percentIn <= 0.2 && pos > 0 && !blendUp)//blend left
		{
			BiomeData bd = biomes[pos];
			const BiomeData &bd2 = biomes[pos - 1];
			float percentKeep = (percentIn) / 0.2;
			float oneMinusPercentKeep = 1 - percentKeep;

			bd.bottomBlockDepth = bd.bottomBlockDepth * percentKeep + bd2.bottomBlockDepth * oneMinusPercentKeep;			
			bd.bottomBlockDepth = bd.bottomBlockDepth < 1 ? 1 : bd.bottomBlockDepth;
			bd.minStonePos = bd.minStonePos * percentKeep + bd2.minStonePos * oneMinusPercentKeep;
			bd.maxStonePos = bd.maxStonePos * percentKeep + bd2.maxStonePos * oneMinusPercentKeep;

			return bd;
		}
		
		return biomes[pos];
	}

	constexpr std::uint32_t  shiftNumber(std::uint32_t  nr)
	{
		std::uint32_t  copy = nr;
		nr <<= nr%3 + 1;
		nr >>= 1;
		nr ^= (nr / 13);
		nr += copy / 4;
		return nr;
	}

	WorldGenerator(std::uint32_t  seed): stoneSeed(seed),
		biomeSeed(shiftNumber(stoneSeed)), heigthSeed(shiftNumber(biomeSeed))
	{
		stoneNoise.reseed(stoneSeed);
		biomeNoise.reseed(biomeSeed);
		biomeHeigthNoise.reseed(heigthSeed);

		setDefaultBiomes();
	};
	
	void setupChunk(Chunk *chunk, glm::vec2 p);

	const std::uint32_t stoneSeed;
	const std::uint32_t biomeSeed;
	const std::uint32_t heigthSeed;

	siv::PerlinNoise stoneNoise;
	siv::PerlinNoise biomeNoise;
	siv::PerlinNoise biomeHeigthNoise;

	///stone pass
	float stoneNoiseCompression = 120;
	float heightNoiseCompression = 80;

	int stoneOctaves = 6;

	//this 2 numbers should not be too big(greater than 250 or too small)
	//also min should be less than max
	//int minStonePos = 90;
	//int maxStonePos = 190;

	float biomeNoiseCompression = 150;

	//this is between 0 and 1
	float stoneChance = 0.4;

	///grass && biome pass
	//this is multiplied by a 0-1 value
	int dirtHeigthCompresion = 40;

};

