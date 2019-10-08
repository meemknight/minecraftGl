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
				ilog(x, yDown, z);
			}
			x++;
		}

		
	}


}