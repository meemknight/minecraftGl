#include "phisics.h"
#include "tools.h"

std::vector<glm::ivec4> blocksToCheck;

void resolveConstrains(glm::vec3 &pos, glm::vec3 lastPos, ChunkManager &cm, glm::vec3 dimensions, CubeWireRenderer *cw)
{
	blocksToCheck.clear();
	glm::vec3 delta = pos - lastPos;

	//colliding downwards
	float yDown = ceil(pos.y - dimensions.y - 1);
	//if(delta.y < 0)
	{
		int x = floorf(pos.x - dimensions.x);
		int z = floorf(pos.z - dimensions.z);
		
		while(x < ceilf(pos.x + dimensions.x))
		{
			while (z < ceilf(pos.z + dimensions.z))
			{
				blocksToCheck.emplace_back(x, yDown, z, 0);
				//cw->addCube({ x, yDown, z }, { 1,0,0,1 });
				z++;
			}
			x++;
		}

		
	}


}

constexpr float rayMarch = 0.1;
glm::vec3 returnedVector;

std::optional<glm::vec3> rayCast(ChunkManager &cm, glm::vec3 position, glm::vec3 direction, float maxLength)
{
	float distanceTraveled = 0;

	direction = glm::normalize(direction);
	direction *= rayMarch;

	glm::vec3 rayPos = position;

	while(distanceTraveled < maxLength)
	{
		Block &b = cm.getBlock(glm::floor(rayPos));
			if (b!=BLOCK::air)
			{
				return glm::floor(rayPos);
			}
			rayPos += direction;
			distanceTraveled += rayMarch;
	}

	//wlog("no");

	return {};
}
