#include "Blocks.h"

static BlockFace frontFaces[BLOCK::BLOCKS_SIZE] =
{
	{0},
	{2, 15}, // dirt
	{3, 15}, // grass
	{1, 15},//stone
	{4, 14},//log
	{4, 15},//wooden_plank
	{0, 14},//cobblestone
	{7, 14},//gold_block
	{7,15},//bricks
};

static BlockFace backFaces[BLOCK::BLOCKS_SIZE] =
{
	{0},
	{2, 15}, // dirt
	{3, 15}, // grass
	{1, 15},//stone
	{4, 14},//log
	{4, 15},//wooden_plank
	{0, 14},//cobblestone
	{7, 14},//gold_block
	{7,15},//bricks

};

static BlockFace topFaces[BLOCK::BLOCKS_SIZE] =
{
	{0},
	{2, 15}, // dirt
	{0, 15}, // grass
	{1, 15},//stone
	{5, 14},//log
	{4, 15},//wooden_plank
	{0, 14},//cobblestone
	{7, 14},//gold_block
	{7,15},//bricks

};

static BlockFace bottomFaces[BLOCK::BLOCKS_SIZE] =
{
	{0},
	{2, 15}, // dirt
	{2, 15}, // grass
	{1, 15},//stone
	{5, 14},//log
	{4, 15},//wooden_plank
	{0, 14},//cobblestone
	{7, 14},//gold_block
	{7,15},//bricks

};

static BlockFace LeftFaces[BLOCK::BLOCKS_SIZE] =
{
	{0},
	{2, 15}, // dirt
	{3, 15}, // grass
	{1, 15},//stone
	{4, 14},//log
	{4, 15},//wooden_plank
	{0, 14},//cobblestone
	{7, 14},//gold_block
	{7,15},//bricks

};

static BlockFace RightFaces[BLOCK::BLOCKS_SIZE] =
{
	{0},
	{2, 15}, // dirt
	{3, 15}, // grass
	{1, 15},//stone
	{4, 14},//log
	{4, 15},//wooden_plank
	{0, 14},//cobblestone
	{7, 14},//gold_block
	{7,15},//bricks

};

static BlockFace *faces[FACE::FACES_SIZE] =
{
	frontFaces, backFaces, topFaces, bottomFaces, LeftFaces, RightFaces,
};

BlockFace getBlockFace(Block b, Face f)
{
	return faces[f][b];
}

//todo is opaque
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

bool isCollideble(Block b)
{
	if (b != BLOCK::air)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
