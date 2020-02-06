#pragma once
#include <glm/vec3.hpp>
#include "phisics.h"
#include "Camera.h"

struct Entity;

struct FlyMoveStruct
{
	Entity *e;
	const FirstPersonCamera &fp;

	void moveUp(float speed);
	void moveDown(float speed);
	void moveLeft(float speed);
	void moveRight(float speed);
	void moveFront(float speed);
	void moveBack(float speed);
};


struct walkMoveStruct
{
	Entity *e;
	const FirstPersonCamera &fp;

	void moveOnDirection(float x, float z, float deltaTime);
};


struct Entity
{
	glm::vec3 position;
	glm::vec3 lastPos;

	void resolveConstrains(ChunkManager &cm, glm::vec3 dimensions, CubeWireRenderer *cw);

	void updatePositions();

	float flySpeed = 30;

	void applyGravity(float deltaTime);

	FlyMoveStruct fly(const FirstPersonCamera &f) { return { this, f }; };
	walkMoveStruct walk(const FirstPersonCamera &f) { return { this, f }; };

};
