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
			if (isOpaque(cm.getBlock(i)))
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
			if (isOpaque(cm.getBlock(i)))
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
			if (isOpaque(cm.getBlock(i)))
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
			if (isOpaque(cm.getBlock(i)))
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
			if (isOpaque(cm.getBlock(i)))
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
				if (isOpaque(cm.getBlock(i)))
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
		if (isOpaque(cm.getBlock(block)))
		{
			pos.x = floorf(block.x + dimensions.x) + 1;
		}
	}else if(delta.x > 0)
	{
		if (isOpaque(cm.getBlock(block)))
		{
			pos.x = floorf(block.x + dimensions.x) - 1;
		}
	}

	if (delta.y < 0) //down
	{
		if (isOpaque(cm.getBlock(block)))
		{
			pos.y = block.y + dimensions.y;
		}
	}else if (delta.y > 0)//up
	{
		if (isOpaque(cm.getBlock(block)))
		{
			pos.y = floorf(block.y) - 1;
		}
	}
	
	if (delta.z < 0) //left
	{
		if (isOpaque(cm.getBlock(block)))
		{
			pos.z = floorf(block.z + dimensions.z) + 1;
		}
	}
	else if (delta.z > 0)
	{
		if (isOpaque(cm.getBlock(block)))
		{
			pos.z = floorf(block.z + dimensions.z) - 1;
		}
	}

}

void resolveConstrainsBrute(glm::vec3 &pos, glm::vec3 lastPos, ChunkManager &cm, glm::vec3 dimensions, CubeWireRenderer *cw, bool *grounded);

//does a smart check for block skipping
void resolveConstrains(glm::vec3 &pos, glm::vec3 lastPos, ChunkManager &cm, glm::vec3 dimensions, CubeWireRenderer *cw, bool *grounded)
{
	float distance = glm::length(lastPos - pos);
	
	if(distance < 1.f)
	{
		resolveConstrainsBrute(pos,
			lastPos,
			cm,
			dimensions,
			cw,
			grounded);
	}
	else
	{
		glm::vec3 newPos = lastPos;
		glm::vec3 delta = pos - lastPos;
		delta = glm::normalize(delta);
		delta *= 0.9;

		do
		{
			newPos += delta;
			glm::vec3 posTest = newPos;
			resolveConstrainsBrute(newPos,
				lastPos,
				cm,
				dimensions,
				cw,
				grounded);

			if (newPos != posTest)
			{
				pos = newPos;
				return;
			}

		} while (glm::length((newPos + delta) - pos) > 1.0f);
		//todo optimize this while
	
		resolveConstrainsBrute(pos,
			lastPos,
			cm,
			dimensions,
			cw,
			grounded);
	}
}

//does only one check, regardless of the distance that the body has travelled
void resolveConstrainsBrute(glm::vec3 &pos, glm::vec3 lastPos, ChunkManager &cm, glm::vec3 dimensions, CubeWireRenderer *cw, bool *grounded) 
{
	constexpr float r = 0.95;

	glm::vec3 position = lastPos;
	glm::vec3 vect = pos - lastPos;
	glm::vec3 newLastPos;

	newLastPos = position;
	position.x += vect.x;
	performCollision(position, newLastPos, cm, { dimensions.x, dimensions.y * r, dimensions.z * r }, cw);

	newLastPos = position;
	position.y += vect.y;
	float testY = position.y;
	performCollision(position, newLastPos, cm, { dimensions.x * r, dimensions.y, dimensions.z * r }, cw);
	if(grounded)
	{
		if (testY == position.y)
		{
			*grounded = 0;
		}else
		{
			*grounded = 1;
		}
	}
	

	newLastPos = position;
	position.z += vect.z;
	performCollision(position, newLastPos, cm, { dimensions.x * r, dimensions.y * r, dimensions.z }, cw);

	pos = position;
}


void performCollision(glm::vec3 &pos, glm::vec3 lastPos, ChunkManager &cm, glm::vec3 dimensions, CubeWireRenderer *cw)
{
	constexpr float topMarj = 0.1;

	dimensions.x /= 2;
	//dimensions.y /= 2;
	dimensions.z /= 2;


	glm::vec3 posCopy = pos;
	//posCopy.x += dimensions.x;
	//posCopy.z += dimensions.z;
	

	glm::vec3 delta = pos - lastPos;

	if(delta.x > 0) // dreapta
	{
		int x = floor(posCopy.x + dimensions.x + 0.5);
		for (int y = ceil(posCopy.y - dimensions.y); y <= floor(posCopy.y - topMarj) + 1; y++)
			for (int z = floor(posCopy.z - dimensions.z + 0.5); z <= floor(posCopy.z + dimensions.z + 0.5); z++)
			{
				glm::ivec3 block = { x,y,z };
				if(cw)
				{
					cw->addCube(block, { 1,0,0,1 });
				}
				if (isOpaque(cm.getBlock(block)))
				{
					pos.x = floorf(block.x + dimensions.x) - 1;
				}
			}
	}
	else if (delta.x < 0)  // stanga
	{
		int x = floor(posCopy.x - dimensions.x + 0.5);
		for (int y = ceil(posCopy.y - dimensions.y); y <= floor(posCopy.y - topMarj) + 1; y++)
			for (int z = floor(posCopy.z - dimensions.z + 0.5); z <= floor(posCopy.z + dimensions.z + 0.5); z++)
			{
				glm::ivec3 block = { x,y,z };
				if (cw)
				{
					cw->addCube(block, { 1,0,0,1 });
				}
				if (isOpaque(cm.getBlock(block)))
				{
					pos.x = floorf(block.x + dimensions.x) + 1;
				}
			}
	}

	if(delta.y > 0) // sus
	{
		int y = floor(posCopy.y) + 1;
		for (int x = floor(posCopy.x - dimensions.x + 0.5); x <= floor(posCopy.x + dimensions.x + 0.5); x++)
			for (int z = floor(posCopy.z - dimensions.z + 0.5); z <= floor(posCopy.z + dimensions.z + 0.5); z++)
			{
				glm::ivec3 block = { x,y,z };
				if (cw)
				{
					cw->addCube(block, { 1,0,0,1 });
				}
				if (isOpaque(cm.getBlock(block)))
				{
					pos.y = block.y - 1;
				}
			}
	
	}else if(delta.y < 0)
	{
		int y = ceil(posCopy.y - dimensions.y);
		for (int x = floor(posCopy.x - dimensions.x + 0.5); x <= floor(posCopy.x + dimensions.x + 0.5); x++)
			for (int z = floor(posCopy.z - dimensions.z + 0.5); z <= floor(posCopy.z + dimensions.z + 0.5); z++)
			{
				glm::ivec3 block = { x,y,z };
				if (cw)
				{
					cw->addCube(block, { 1,0,0,1 });
				}
				if (isOpaque(cm.getBlock(block)))
				{
					pos.y = block.y + dimensions.y;
				}
			}
	
	}

	if (delta.z > 0) // fata
	{
		int z = floor(posCopy.z + dimensions.z + 0.5);
		for (int x = floor(posCopy.x - dimensions.x + 0.5); x <= floor(posCopy.x + dimensions.x + 0.5); x++)
			for (int y = ceil(posCopy.y - dimensions.y); y <= floor(posCopy.y - topMarj) + 1; y++)
			{
				glm::ivec3 block = { x,y,z };
				if (cw)
				{
					cw->addCube(block, { 1,0,0,1 });
				}
				if (isOpaque(cm.getBlock(block)))
				{
					pos.z = floorf(block.z + dimensions.z) - 1;
				}
			}

	
	}else if (delta.z < 0)
	{
		int z = floor(posCopy.z - dimensions.z + 0.5);
		for (int x = floor(posCopy.x - dimensions.x + 0.5); x <= floor(posCopy.x + dimensions.x + 0.5); x++)
			for (int y = ceil(posCopy.y - dimensions.y); y <= floor(posCopy.y - topMarj) + 1; y++)
			{
				glm::ivec3 block = { x,y,z };
				if (cw)
				{
					cw->addCube(block, { 1,0,0,1 });
				}
				if (isOpaque(cm.getBlock(block)))
				{
					pos.z = floorf(block.z + dimensions.z) + 1;
				}
			}
	
	}

}

/*
void resolveConstrains(glm::vec3 &pos, glm::vec3 lastPos, ChunkManager &cm, glm::vec3 dimensions, CubeWireRenderer *cw)
{
	dimensions.x /= 2;
	//dimensions.y /= 2;
	dimensions.z /= 2;


	glm::vec3 posCopy = pos;
	//posCopy.x += dimensions.x;
	//posCopy.z += dimensions.z;


	glm::vec3 delta = pos - lastPos;

	for (int x = floor(posCopy.x - dimensions.x + 0.5); x <= floor(posCopy.x + dimensions.x + 0.5); x++)
		for (int y = ceil(posCopy.y - dimensions.y); y <= floor(posCopy.y) + 1; y++)
			for (int z = floor(posCopy.z - dimensions.z + 0.5); z <= floor(posCopy.z + dimensions.z + 0.5); z++)
			{
				resolveConstrainsOnBlock(pos, lastPos, cm, dimensions, cw, delta, { x,y,z });
			}

}
*/

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
		b = BLOCK::air;
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
				//todo when go to the limit solve ghostblock
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

