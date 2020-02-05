#include "Entity.h"

void Entity::resolveConstrains(ChunkManager & cm, glm::vec3 dimensions, CubeWireRenderer * cw)
{
	::resolveConstrains(position, lastPos, cm, dimensions, cw);
}

void Entity::updatePositions()
{
	lastPos = position;
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

