#pragma once
#include "PerlinNoise.hpp"
#include "Chunk.h"
#include <glm/vec2.hpp>
#include "Blocks.h"
#include "FastNoiseSIMD/FastNoiseSIMD.h"
#include "ChunkManager.h"

struct WorldGenerator
{
	struct BiomeData;
private:
	std::vector<BiomeData> biomes;
	int chanceCounter = 0;
public:

	bool biomeBlend = true;

	struct BiomeData
	{
		void averageWith(BiomeData o)
		{
			bottomBlockDepth = (bottomBlockDepth + o.bottomBlockDepth) / 2.f;
			minStonePos = (minStonePos + o.minStonePos) / 2.f;
			maxStonePos = (maxStonePos + o.maxStonePos) / 2.f;
			stoneChance = (stoneChance + o.stoneChance) / 2.f;
			realismExponent = (realismExponent + o.realismExponent) / 2.f;
			octaves = (octaves + o.octaves) / 2.f;
		}

		void combineWith(BiomeData bd2, float percentKeep)
		{
			float oneMinusPercentKeep = 1 - percentKeep;
			bottomBlockDepth = bottomBlockDepth * percentKeep + bd2.bottomBlockDepth * oneMinusPercentKeep;
			bottomBlockDepth = bottomBlockDepth < 1 ? 1 : bottomBlockDepth;
			minStonePos = minStonePos * percentKeep + bd2.minStonePos * oneMinusPercentKeep;
			maxStonePos = maxStonePos * percentKeep + bd2.maxStonePos * oneMinusPercentKeep;
			stoneChance = stoneChance * percentKeep + bd2.stoneChance * oneMinusPercentKeep;
			realismExponent = realismExponent * percentKeep + bd2.realismExponent * oneMinusPercentKeep;
			octaves = octaves * percentKeep + bd2.octaves * oneMinusPercentKeep;
		}

		int chanceFactor;
		Block topBlock;
		Block bottomBlock;
		float bottomBlockDepth;
		float minStonePos;
		float maxStonePos;
		float stoneChance;
		//higher means less floaty islands
		float realismExponent;
		float octaves;
	};

	void setDefaultBiomes()
	{

		//crazy desert
		//biomes.push_back({});
		//biomes[biomes.size() - 1].chanceFactor = 1;
		//biomes[biomes.size() - 1].topBlock = BLOCK::sand;
		//biomes[biomes.size() - 1].bottomBlock = BLOCK::sand_stone;
		//biomes[biomes.size() - 1].bottomBlockDepth = 5;
		//biomes[biomes.size() - 1].minStonePos = 90;
		//biomes[biomes.size() - 1].maxStonePos = 160;
		//biomes[biomes.size() - 1].stoneChance = 0.5;
		//biomes[biomes.size() - 1].realismExponent = 0.8;
		//biomes[biomes.size() - 1].octaves = 7;

		//desert
		biomes.push_back({});
		biomes[biomes.size()-1].chanceFactor = 1;
		biomes[biomes.size()-1].topBlock = BLOCK::sand;
		biomes[biomes.size()-1].bottomBlock = BLOCK::sand_stone;
		biomes[biomes.size()-1].bottomBlockDepth = 15;
		biomes[biomes.size()-1].minStonePos = 90;
		biomes[biomes.size()-1].maxStonePos = 120;
		biomes[biomes.size() - 1].stoneChance = 0.6;
		biomes[biomes.size() - 1].realismExponent = 2;
		biomes[biomes.size() - 1].octaves = 2;

		//plains
		biomes.push_back({});
		biomes[biomes.size() - 1].chanceFactor = 1;
		biomes[biomes.size() - 1].topBlock = BLOCK::grass;
		biomes[biomes.size() - 1].bottomBlock = BLOCK::dirt;
		biomes[biomes.size() - 1].bottomBlockDepth = 8;
		biomes[biomes.size() - 1].minStonePos = 90;
		biomes[biomes.size() - 1].maxStonePos = 130;
		biomes[biomes.size() - 1].stoneChance = 0.5;
		biomes[biomes.size() - 1].realismExponent = 2;
		biomes[biomes.size() - 1].octaves = 6;

		//ice
		biomes.push_back({});
		biomes[biomes.size() - 1].chanceFactor = 1;
		biomes[biomes.size() - 1].topBlock = BLOCK::snow_dirt;
		biomes[biomes.size() - 1].bottomBlock = BLOCK::dirt;
		biomes[biomes.size() - 1].bottomBlockDepth = 6;
		biomes[biomes.size() - 1].minStonePos = 120;
		biomes[biomes.size() - 1].maxStonePos = 200;
		biomes[biomes.size() - 1].stoneChance = 0.4;
		biomes[biomes.size() - 1].realismExponent = 2;
		biomes[biomes.size() - 1].octaves = 6;

		//ice plains
		//biomes.push_back({});
		//biomes[biomes.size() - 1].chanceFactor = 2;
		//biomes[biomes.size() - 1].topBlock = BLOCK::snow_dirt;
		//biomes[biomes.size() - 1].bottomBlock = BLOCK::dirt;
		//biomes[biomes.size() - 1].bottomBlockDepth = 4;
		//biomes[biomes.size() - 1].minStonePos = 140;
		//biomes[biomes.size() - 1].maxStonePos = 180;
		//biomes[biomes.size() - 1].stoneChance = 0.2;
		//biomes[biomes.size() - 1].realismExponent = 3;

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
		
		if(percentIn >= 1- blendTresshold && pos < biomes.size()-1)//blend right
		{
			BiomeData bd = biomes[pos];
			BiomeData bd2 = biomes[pos + 1];
			
			bd2.averageWith(bd);
			
			float percentKeep = 1- (percentIn - (1 - blendTresshold)) / blendTresshold;

			bd.combineWith(bd2, percentKeep);
			//blend up
			return bd;
		}else
		if (percentIn <= blendTresshold && pos > 0)//blend left
		{
			BiomeData bd = biomes[pos];
			BiomeData bd2 = biomes[pos - 1];
			bd2.averageWith(bd);

			float percentKeep = (percentIn) / blendTresshold;
			float oneMinusPercentKeep = 1 - percentKeep;

			bd.combineWith(bd2, percentKeep);

			return bd;
		}
		
		return biomes[pos];
	}

	//https://en.wikipedia.org/wiki/Lehmer_random_number_generator
	constexpr unsigned long long Lehmer(std::uint32_t state)
	{
		state = (state * 48271) % 2147483647;
		return state % 100 + 1;
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

	//this 2 numbers should not be too big(greater than 250 or too small)
	//also min should be less than max
	//int minStonePos = 90;
	//int maxStonePos = 190;

	float biomeNoiseCompression = 150;

	///grass && biome pass
	//this is multiplied by a 0-1 value
	int dirtHeigthCompresion = 40;

	float blendTresshold = 0.25;

};

void generateStructure(ChunkManager &cm, Block *blocks, glm::ivec3 pos, glm::ivec3 size, bool placeOverAnything, bool placeAir);