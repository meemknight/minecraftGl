#include "Entity.h"

void Entity::resolveConstrains(ChunkManager & cm, glm::vec3 dimensions, CubeWireRenderer * cw)
{
	::resolveConstrains(position, lastPos, cm, dimensions, cw, &grounded);
}

void Entity::updatePositions()
{
	lastPos = position;
}

void Entity::applyGravity(float deltaTime)
{
	velocity.y -= deltaTime * 32;
}

void Entity::applyVelocity(float deltaTime)
{
	const float c = 40;
	velocity = glm::clamp(velocity, { -c,-c, -c }, { c, c, c });

	position += velocity * deltaTime;

	//drag
	velocity.x += velocity.x * (-5.0f * deltaTime);
	velocity.z += velocity.z * (-5.0f * deltaTime);

	if(std::fabs(velocity.x) < 0.01)
	{velocity.x = 0;}

	if (std::fabs(velocity.y) < 0.01)
	{velocity.y = 0;}

	if (std::fabs(velocity.z) < 0.01)
	{velocity.z = 0;}

	if(grounded && velocity.y < 0)
	{
		velocity.y = 0;
	}

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

void WalkMoveStruct::moveOnDirection(float x, float z, float deltaTime)
{
	float angle = fp.getTopDownAngle();

	if((x == z) && x == 0)
	{
		return;
	}

	glm::vec2 dir = { x,z };
	if(dir.length() > 1.f)
	{
		dir = glm::normalize(dir);
	}

	glm::vec2 dirMove = { dir.y, dir.x };
	dirMove = glm::mat2x2{ cos(angle), sin(angle), -sin(angle), cos(angle) } * dirMove;

	dirMove *= deltaTime * e->flySpeed;

	e->position.x += dirMove.x;
	e->position.z += dirMove.y;
}

void WalkMoveStruct::jump()
{
	if(e->grounded)
	e->velocity.y = e->jumpSpeed;
}
