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
		BLOCKS_SIZE,
	};
}

struct BlockFace
{
	int x = 0;
	int y = 0;
};

BlockFace getBlockFace(Block b, Face f);