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
	{2,14},//sand
	{0,3},//sand_stone
	{4,11},//snow_dirt
	{5, 12},//leaves
	{7, 13},//herbs
	{14, 14},//sappling
	{7, 12},//dead_bush

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
	{2,14},//sand
	{0,3},//sand_stone
	{4,11},//snow_dirt
	{5, 12},//leaves
	{7, 13},//herbs
	{14, 14},//sappling
	{7, 12},//dead_bush

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
	{2,14},//sand
	{0,4},//sand_stone
	{2, 11}, // snow_grass
	{5, 12},//leaves
	{7, 13},//herbs
	{14, 14},//sappling
	{7, 12},//dead_bush

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
	{2,14},//sand
	{0,2},//sand_stone
	{2, 15}, // snow_grass
	{5, 12},//leaves
	{7, 13},//herbs
	{14, 14},//sappling
	{7, 12},//dead_bush

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
	{2,14},//sand
	{0,3},//sand_stone
	{4,11},//snow_dirt
	{5, 12},//leaves
	{7, 13},//herbs
	{14, 14},//sappling
	{7, 12},//dead_bush

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
	{2,14},//sand
	{0,3},//sand_stone
	{4,11},//snow_dirt
	{5, 12},//leaves
	{7, 13},//herbs
	{14, 14},//sappling
	{7, 12},//dead_bush

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
bool isOpaque(Block b)
{
	if(b!=BLOCK::air)
	{
		if (isHerb(b))
		{
			return 0;
		}

		return 1;
	}else
	{
		return 0;
	}
}

bool isHerb(Block b)
{
	if (b == BLOCK::herb || b == BLOCK::sappling || b == BLOCK::dead_bush)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


bool isCollideble(Block b)
{
	if (b != BLOCK::air)
	{
		if (isHerb(b))
		{
			return 0;
		}

		return 1;
	}
	else
	{
		
		return 0;
	}
}
