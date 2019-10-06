#include "Blocks.h"

static BlockFace frontFaces[BLOCK::BLOCKS_SIZE] =
{
	{0},
	{2, 15}, // dirt
	{3, 15}, // grass
	{1, 15},//stone
};

static BlockFace backFaces[BLOCK::BLOCKS_SIZE] =
{
	{0},
	{2, 15}, // dirt
	{3, 15}, // grass
	{1, 15},//stone
};

static BlockFace topFaces[BLOCK::BLOCKS_SIZE] =
{
	{0},
	{2, 15}, // dirt
	{0, 15}, // grass
	{1, 15},//stone
};

static BlockFace bottomFaces[BLOCK::BLOCKS_SIZE] =
{
	{0},
	{2, 15}, // dirt
	{2, 15}, // grass
	{1, 15},//stone

};

static BlockFace LeftFaces[BLOCK::BLOCKS_SIZE] =
{
	{0},
	{2, 15}, // dirt
	{3, 15}, // grass
	{1, 15},//stone
};

static BlockFace RightFaces[BLOCK::BLOCKS_SIZE] =
{
	{0},
	{2, 15}, // dirt
	{3, 15}, // grass
	{1, 15},//stone

};

static BlockFace *faces[FACE::FACES_SIZE] =
{
	frontFaces, backFaces, topFaces, bottomFaces, LeftFaces, RightFaces,
};

BlockFace getBlockFace(Block b, Face f)
{
	return faces[f][b];
}

bool isSolid(Block b)
{
	if(b!=BLOCK::air)
	{
		return 1;
	}else
	{
		return 0;
	}
}