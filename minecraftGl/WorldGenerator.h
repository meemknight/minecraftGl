#pragma once
#include "PerlinNoise.hpp"
#include "Chunk.h"
#include <glm/vec2.hpp>

struct WorldGenerator
{
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
	int minStonePos = 90;
	int maxStonePos = 190;

	//this is between 0 and 1
	float stoneChance = 0.4;

	///grass && biome pass
	//this is multiplied by a 0-1 value
	int dirtHeigth = 6; 
	int dirtHeigthCompresion = 80;
};

