#include "Entity.h"

void Entity::resolveConstrains(ChunkManager & cm, glm::vec3 dimensions, CubeWireRenderer * cw)
{
	::resolveConstrains(position, lastPos, cm, dimensions, cw);
}

void Entity::updatePositions()
{
	lastPos = position;
}

void Entity::applyGravity(float deltaTime)
{
	position.y -= deltaTime;
}

void FlyMoveStruct::moveUp(float speed)
{
	e->position += glm::vec3{0,1,0} *speed * e->flySpeed;
}

void FlyMoveStruct::moveDown(float speed)
{
	moveUp(-speed);
}

void FlyMoveStruct::moveLeft(float speed)
{
	moveRight(-speed);
}

void FlyMoveStruct::moveRight(float speed)
{
	e->position += glm::normalize(glm::cross(fp.viewDirection, glm::vec3{ 0,1,0 })) * speed * e->flySpeed;
}

void FlyMoveStruct::moveFront(float speed)
{
	e->position += fp.viewDirection * speed * e->flySpeed;
}

void FlyMoveStruct::moveBack(float speed)
{
	moveFront(-speed);
}

void walkMoveStruct::moveOnDirection(float x, float z, float deltaTime)
{
	float angle = fp.getTopDownAngle();

	if((x == z) && x == 0)
	{
		return;
	}

	glm::vec2 dir = { x,z };
	dir = glm::normalize(dir);

	glm::vec2 dirMove = { dir.y, dir.x };
	dirMove = glm::mat2x2{ cos(angle), sin(angle), -sin(angle), cos(angle) } * dirMove;

	dirMove *= deltaTime * e->flySpeed;

	e->position.x += dirMove.x;
	e->position.z += dirMove.y;
}
