#pragma once

using Block = unsigned int;
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
		BLOCKS_SIZE,
	};
}

struct BlockFace
{
	float x = 0;
	float y = 0;
};

BlockFace getBlockFace(Block b, Face f);
bool isSolid(Block b);
bool isCollideble(Block b);