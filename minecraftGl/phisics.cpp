#include "phisics.h"
#include "tools.h"

#define withDir( component, begin, end, direction) (int component = (direction) ? (begin) : (end); (direction) ? (component) < (end) : (component) > (begin); (direction) ? (component)++ : (component)--)

static std::vector<glm::ivec4> blocksToCheck;

static void resolveConstrainsOnX(glm::vec3 &pos, glm::vec3 lastPos, ChunkManager &cm, glm::vec3 dimensions, CubeWireRenderer *cw, glm::vec3 delta)
{
	glm::vec3 posCopy = pos;
	posCopy.x += 0.5f;
	posCopy.z += 0.5f;

	blocksToCheck.clear();
	if (delta.x < 0)
	{
		float xFront = ceil(posCopy.x - dimensions.x) - 1;

		int y = posCopy.y - dimensions.y / 2;
		int z;

		while (y < ceilf(posCopy.y + dimensions.y / 2))
		{
			z = floorf(posCopy.z - dimensions.z);
			while (z < ceilf(posCopy.z + dimensions.z))
			{
				blocksToCheck.emplace_back(xFront, y, z, 0);
				cw->addCube({ xFront, y, z }, { 1,0,0,1 });
				z++;
			}
			y++;
		}

		for (auto &i : blocksToCheck)
		{
			if (isSolid(cm.getBlock(i)))
			{
				pos.x = floorf(i.x + dimensions.x) + 1;
				break;
			}
		}

	}
	else
	if (delta.x > 0)
	{
		float xBack = ceil(posCopy.x - dimensions.x);

		int y = posCopy.y - dimensions.y / 2;
		int z;

		while (y < ceilf(posCopy.y + dimensions.y / 2))
		{
			z = floorf(posCopy.z - dimensions.z);
			while (z < ceilf(posCopy.z + dimensions.z))
			{
				blocksToCheck.emplace_back(xBack, y, z, 0);
				cw->addCube({ xBack, y, z }, { 1,0,0,1 });
				z++;
			}
			y++;
		}

		for (auto &i : blocksToCheck)
		{
			if (isSolid(cm.getBlock(i)))
			{
				pos.x = floorf(i.x + dimensions.x) - 1;
				break;
			}
		}

	}
}

static void resolveConstrainsOnY(glm::vec3 &pos, glm::vec3 lastPos, ChunkManager &cm, glm::vec3 dimensions, CubeWireRenderer *cw, glm::vec3 delta)
{
	glm::vec3 posCopy = pos;
	posCopy.x += 0.5f;
	posCopy.z += 0.5f;

	blocksToCheck.clear();
	if (delta.y < 0)//down
	{
		float yDown = ceil(posCopy.y - dimensions.y);

		int x = floorf(posCopy.x - dimensions.x);
		int z;

		while (x < ceilf(posCopy.x + dimensions.x))
		{
			z = floorf(posCopy.z - dimensions.z);
			while (z < ceilf(posCopy.z + dimensions.z))
			{
				blocksToCheck.emplace_back(x, yDown, z, 0);
				cw->addCube({ x, yDown, z }, { 1,0,0,1 });
				z++;
			}
			x++;
		}


		for (auto &i : blocksToCheck)
		{
			if (isSolid(cm.getBlock(i)))
			{
				pos.y = i.y + dimensions.y;
				break;
			}
		}

	}
	else
	if (delta.y > 0)//up
	{
		float yUp = floor(posCopy.y) + 1;

		int x = floorf(posCopy.x - dimensions.x);
		int z;

		while (x < ceilf(posCopy.x + dimensions.x))
		{
			z = floorf(posCopy.z - dimensions.z);
			while (z < ceilf(posCopy.z + dimensions.z))
			{
				blocksToCheck.emplace_back(x, yUp, z, 0);
				cw->addCube({ x, yUp, z }, { 1,0,0,1 });
				z++;
			}
			x++;
		}


		for (auto &i : blocksToCheck)
		{
			if (isSolid(cm.getBlock(i)))
			{
				pos.y = i.y - 1;
				break;
			}
		}
	}

}

static void resolveConstrainsOnZ(glm::vec3 &pos, glm::vec3 lastPos, ChunkManager &cm, glm::vec3 dimensions, CubeWireRenderer *cw, glm::vec3 delta)
{
	glm::vec3 posCopy = pos;
	posCopy.x += 0.5f;
	posCopy.z += 0.5f;

	blocksToCheck.clear();
	if (delta.z < 0)
	{
		float zFront = ceil(posCopy.z - dimensions.z) - 1;

		int y = posCopy.y - dimensions.y / 2;
		int x;

		while (y < ceilf(posCopy.y + dimensions.y / 2))
		{
			x = floorf(posCopy.x - dimensions.x);
			while (x < ceilf(posCopy.x + dimensions.x))
			{
				blocksToCheck.emplace_back(x, y, zFront, 0);
				cw->addCube({ x, y, zFront }, { 1,0,0,1 });
				x++;
			}
			y++;
		}

		for (auto &i : blocksToCheck)
		{
			if (isSolid(cm.getBlock(i)))
			{
				pos.z = floorf(i.z + dimensions.z) + 1;
				break;
			}
		}

	}
	else
		if (delta.z > 0)
		{

			float zBack = ceil(posCopy.z - dimensions.z);

			int y = posCopy.y - dimensions.y / 2;
			int x;

			while (y < ceilf(posCopy.y + dimensions.y / 2))
			{
				x = floorf(posCopy.x - dimensions.x);
				while (x < ceilf(posCopy.x + dimensions.x))
				{
					blocksToCheck.emplace_back(x, y, zBack, 0);
					cw->addCube({ x, y, zBack }, { 1,0,0,1 });
					x++;
				}
				y++;
			}

			for (auto &i : blocksToCheck)
			{
				if (isSolid(cm.getBlock(i)))
				{
					pos.z = floorf(i.z + dimensions.z) - 1;
					break;
				}
			}

		}
}

static void resolveConstrainsOnBlock(
	glm::vec3 &pos,
	glm::vec3 lastPos,
	ChunkManager &cm,
	glm::vec3 dimensions,
	CubeWireRenderer *cw,
	glm::vec3 delta,
	glm::ivec3 block)
{
	cw->addCube(block, { 1,0,0,1 });

	if (delta.x < 0) //left
	{
		if (isSolid(cm.getBlock(block)))
		{
			pos.x = floorf(block.x + dimensions.x) + 1;
		}
	}else if(delta.x > 0)
	{
		if (isSolid(cm.getBlock(block)))
		{
			pos.x = floorf(block.x + dimensions.x) - 1;
		}
	}

	if (delta.y < 0) //down
	{
		if (isSolid(cm.getBlock(block)))
		{
			pos.y = block.y + dimensions.y;
		}
	}else if (delta.y > 0)//up
	{
		if (isSolid(cm.getBlock(block)))
		{
			pos.y = floorf(block.y) - 1;
		}
	}
	
	if (delta.z < 0) //left
	{
		if (isSolid(cm.getBlock(block)))
		{
			pos.z = floorf(block.z + dimensions.z) + 1;
		}
	}
	else if (delta.z > 0)
	{
		if (isSolid(cm.getBlock(block)))
		{
			pos.z = floorf(block.z + dimensions.z) - 1;
		}
	}

}


void resolveConstrains(glm::vec3 &pos, glm::vec3 lastPos, ChunkManager &cm, glm::vec3 dimensions, CubeWireRenderer *cw)
{
	dimensions.x /= 2;
	//dimensions.y /= 2;
	dimensions.z /= 2;

	glm::vec3 delta = pos - lastPos;

	glm::vec3 posCopy = pos;
	//posCopy.x += dimensions.x;
	//posCopy.z += dimensions.z;
	
	//foreach(x,y,functionPtr,boolDir)

	for (int x = floor(posCopy.x - dimensions.x + 0.5); x <= floor(posCopy.x + dimensions.x + 0.5); x++)
		for (int y = ceil(posCopy.y - dimensions.y); y <= floor(posCopy.y) + 1; y++)
			for (int z = floor(posCopy.z - dimensions.z + 0.5); z <= floor(posCopy.z + dimensions.z + 0.5); z++)
			{
				resolveConstrainsOnBlock(pos, lastPos, cm, dimensions, cw, delta, { x,y,z });
			}

	//for withDir(x, ceil(posCopy.x - dimensions.x)-1, ceil(posCopy.x - dimensions.x), delta.x < 0)
	//{
	//	for withDir(y, ceil(posCopy.y - dimensions.y), floor(posCopy.y) + 1, delta.y<0)
	//	{
	//		for withDir(z, ceil(posCopy.z - dimensions.z) - 1, ceil(posCopy.z - dimensions.z), delta.z < 0)
	//		{
	//			resolveConstrainsOnBlock(pos, lastPos, cm, dimensions, cw, delta, { x,y,z });
	//		}
	//	}
	//}

	//resolveConstrainsOnY(pos, lastPos, cm, dimensions, cw, delta);
}

constexpr float rayMarch = 0.01;
glm::vec3 returnedVector;

///probably deprecated
std::optional<glm::vec3> rayCast(ChunkManager &cm, glm::vec3 position, glm::vec3 direction, float maxLength)
{
	float distanceTraveled = 0;

	direction = glm::normalize(direction);
	direction *= rayMarch;

	glm::vec3 rayPos = position;

	while(distanceTraveled < maxLength)
	{
		Block b = cm.getBlock(glm::floor(rayPos));
			if (b!=BLOCK::air)
			{
				return glm::floor(rayPos);
			}
			rayPos += direction;
			distanceTraveled += rayMarch;
	}

	return {};
}

///probably deprecated
std::optional<glm::vec3> rayCastEdge(ChunkManager & cm, glm::vec3 position, glm::vec3 direction, float maxLength)
{
	float distanceTraveled = 0;

	direction = glm::normalize(direction);
	direction *= rayMarch;

	glm::vec3 rayPos = position;

	glm::ivec3 curentPos = glm::floor(rayPos);
	std::optional<glm::ivec3> lastPos = {};

	while (distanceTraveled < maxLength)
	{
		Block b = cm.getBlock(curentPos);
		if (b != BLOCK::air)
		{
			return lastPos;
		}
		rayPos += direction;
		distanceTraveled += rayMarch;

		if(curentPos != glm::ivec3(glm::floor(rayPos)))
		{
			lastPos = curentPos;
			curentPos = glm::floor(rayPos);
		}
	}

	return {};
}

void rayCastAdvanced(ChunkManager & cm, glm::vec3 position, glm::vec3 direction, float maxLength,
	std::optional<glm::ivec3> &collide, std::optional<glm::ivec3> &edge)
{
	float distanceTraveled = 0;
	int repetitions = 1;

	direction = glm::normalize(direction);
	direction *= rayMarch;

	glm::vec3 rayPos = position;

	glm::ivec3 curentPos = glm::floor(rayPos);
	std::optional<glm::ivec3> lastPos = {};

	Block b;
	if(curentPos.y >= 0 && curentPos.y < BUILD_LIMIT)
	{
		b = cm.getBlock(curentPos);
	}else
	{
		collide = {};
		edge = {};
	}

	while (distanceTraveled < maxLength)
	{
		if (b != BLOCK::air)
		{
			edge = lastPos;
			collide = curentPos;
			return;
		}

		rayPos += direction;
		distanceTraveled += rayMarch;

		if (curentPos != glm::ivec3(glm::floor(rayPos)))
		{
			lastPos = curentPos;
			curentPos = glm::floor(rayPos);
			if (curentPos.y >= 0 && curentPos.y < BUILD_LIMIT)
			{
				b = cm.getBlock(curentPos);
			}
			else
			{
				collide = {};
				edge = {};
			}
		}
	}

	collide = {};
	edge = {};

	return;
}

