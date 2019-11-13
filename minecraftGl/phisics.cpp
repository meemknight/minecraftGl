#include "phisics.h"
#include "tools.h"

static std::vector<glm::ivec4> blocksToCheck;

void resolveConstrains(glm::vec3 &pos, glm::vec3 lastPos, ChunkManager &cm, glm::vec3 dimensions, CubeWireRenderer *cw)
{
	dimensions.x /= 2;
	//dimensions.y /= 2;
	dimensions.z /= 2;

	blocksToCheck.clear();
	glm::vec3 delta = pos - lastPos;

	//colliding downwards
	glm::vec3 posCopy = pos;
	posCopy.x += 0.5f;
	posCopy.z += 0.5f;

	float yDown = ceil(posCopy.y - dimensions.y);
	if(delta.y < 0)
	{
		int x = floorf(posCopy.x - dimensions.x);
		int z;
		
		while(x < ceilf(posCopy.x + dimensions.x))
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

	
		for(auto &i : blocksToCheck)
		{
			if (isSolid(cm.getBlock(i)))
			{
				pos.y = floorf(i.y + dimensions.y + 1);
				break;
			}
		}

	}


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

