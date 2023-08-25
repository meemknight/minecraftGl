/////////////////////////////////////////////
//Camera.h
//Copyright(c) 2019 Luta Vlad
//https://github.com/meemknight/OpenGLEngine
/////////////////////////////////////////////

#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Windows.h>
#include <vector>

#include "Chunk.h"
#include "Frustum.h"

//todo remove camera movement probably
struct FirstPersonCamera
{
	FirstPersonCamera() {};
	FirstPersonCamera(float fov, float closePlane, float farPlane, int *width, int *height);

	glm::vec3 viewDirection = { 0.f, 0.f, -1.f };
	const glm::vec3 upPositipon = { 0, 1, 0 };
	glm::vec3 position = { 0,0,0 };

	float fov;
	float closePlane;
	float farPlane;
	int *width; /// pointer to the idth of the screen
	int *height; /// pointer to the height of the screen

	glm::mat4 getProjectionViewMatrix(); /// gets the full view transform
	glm::mat4 getObjectToWorldMatrix(); ///gets the object to world for the camera
	glm::mat4 getProjectionMatrix();

	float speed = 30;

	void moveUp(float speed);
	void moveDown(float speed);
	void moveLeft(float speed);
	void moveRight(float speed);
	void moveFront(float speed);
	void moveBack(float speed);

	void updatePosition(glm::vec2 delta);

	float getTopDownAngle() const;

	float rSpeed = 0.22f;

	void getChunksInFrustrum(std::vector<glm::ivec3> &chunksToLoad);

private:
	glm::vec2 oldMousePosition = {};
};