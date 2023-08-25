#pragma once

using Block = unsigned char;
using Face = int;

enum FACE : int
{
	front = 0,
	back,
	top,
	bottom,
	left,
	right,
	FACES_SIZE,
};
namespace BLOCK
{
	enum : Block
	{
		air = 0,
		dirt,
		grass,
		stone,
		log,
		wooden_plank,
		cobblestone,
		gold_block,
		bricks,
		sand,
		sand_stone,
		snow_dirt,
		leaves,
		herb,
		sappling,
		dead_bush,
		gold_ore,
		coar_ore,
		shugarCane,
		stoneBrick,
		iron_ore,
		diamond_ore,
		bookShelf,
		birch_wood,
		gravel,
		rose,
		ice,
		borderBlock,
		BLOCKS_SIZE,
	};
}

//todo change ints
struct BlockFace
{
	float x = 0;
	float y = 0;
};

BlockFace getBlockFace(Block b, Face f);
bool isOpaque(Block b);
bool isCollideble(Block b);
bool isHerb(Block b);