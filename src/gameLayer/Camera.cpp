/////////////////////////////////////////////
//Camera.cpp
//Copyright(c) 2019 Luta Vlad
//https://github.com/meemknight/OpenGLEngine
/////////////////////////////////////////////

#include "Camera.h"
#include <math.h>
#include "tools.h"
//#include <Windows.h>

#include "Frustum.h"

static bool isPositive(float n)
{
	return n >= 0;
}

void FirstPersonCamera::getChunksInFrustrum(std::vector<glm::ivec3>& chunksToLoad)
{

	/*
	glm::vec2 nearDimensions{};
	glm::vec2 farDimensions{};
	glm::vec3 centerNear{};
	glm::vec3 centerFar{};

	computeFrustumDimensions(position, viewDirection, glm::radians(fov), (float)*width / (float)*height,
		closePlane, farPlane, nearDimensions, farDimensions, centerNear, centerFar);

	glm::vec3 nearTopLeft{};
	glm::vec3 nearTopRight{};
	glm::vec3 nearBottomLeft{};
	glm::vec3 nearBottomRight{};
	glm::vec3 farTopLeft{};
	glm::vec3 farTopRight{};
	glm::vec3 farBottomLeft{};
	glm::vec3 farBottomRight{};

	computeFrustumSplitCorners(viewDirection, nearDimensions, farDimensions, centerNear, centerFar,
		nearTopLeft,
		nearTopRight,
		nearBottomLeft,
		nearBottomRight,
		farTopLeft,
		farTopRight,
		farBottomLeft,
		farBottomRight
	);


	glm::vec3 corners[] =
	{
		nearTopLeft,
		nearTopRight,
		nearBottomLeft,
		nearBottomRight,
		farTopLeft,
		farTopRight,
		farBottomLeft,
		farBottomRight,
	};

	//float longestDiagonal = glm::distance(nearTopLeft, farBottomRight);

	glm::vec3 minVal{};
	glm::vec3 maxVal{};

	for (int i = 0; i < 8; i++)
	{
		glm::vec4 corner(corners[i], 1);


		if (i == 0)
		{
			minVal = corner;
			maxVal = corner;
		}
		else
		{
			if (corner.x < minVal.x) { minVal.x = corner.x; }
			if (corner.y < minVal.y) { minVal.y = corner.y; }
			if (corner.z < minVal.z) { minVal.z = corner.z; }

			if (corner.x > maxVal.x) { maxVal.x = corner.x; }
			if (corner.y > maxVal.y) { maxVal.y = corner.y; }
			if (corner.z > maxVal.z) { maxVal.z = corner.z; }

		}

	}


	for (int z = minVal.z / CHUNK_SIZE; z < maxVal.z / CHUNK_SIZE; z++)
	{
		for (int x = minVal.x / CHUNK_SIZE; x < maxVal.x / CHUNK_SIZE; x++)
		{
			chunksToLoad.emplace_back(x, 0, z);
		}
	}
	*/

	
	float halfA = glm::radians(fov) / 1.5f;
	float angle = getTopDownAngle();
	glm::vec2 pos[4] = { glm::vec2(position.x, position.z) };
	pos[1] = pos[0] + (glm::vec2(cos(angle), sin(angle)) * farPlane);
	pos[2] = pos[0] + (glm::vec2(cos(angle + halfA), sin(angle + halfA)) * farPlane);
	pos[3] = pos[0] + (glm::vec2(cos(angle - halfA), sin(angle - halfA)) * farPlane);

	float minX = pos[0].x;
	float minY = pos[0].y;
	float maxX = pos[0].x;
	float maxY = pos[0].y;

	for(int i = 1; i<4;i++)
	{
		if(pos[i].x > maxX)
		{
			maxX = pos[i].x;
		}

		if (pos[i].y > maxY)
		{
			maxY = pos[i].y;
		}

		if (pos[i].x < minX)
		{
			minX = pos[i].x;
		}

		if (pos[i].y < minY)
		{
			minY = pos[i].y;
		}
	}

	for(int y=minY/ CHUNK_SIZE - 2; y<maxY/ CHUNK_SIZE + 2;y++)
	{
		for(int x = minX/ CHUNK_SIZE - 2; x<maxX/ CHUNK_SIZE + 2; x++)
		{
			chunksToLoad.emplace_back(x, 0, y);
		}
	}
	

}


FirstPersonCamera::FirstPersonCamera(float fov, float closePlane, float farPlane, int * width, int * height)
:fov(fov), closePlane(closePlane), farPlane(farPlane), width(width), height(height)
{
}

glm::mat4 FirstPersonCamera::getProjectionViewMatrix()
{
	return getProjectionMatrix() * getObjectToWorldMatrix();
}

glm::mat4 FirstPersonCamera::getProjectionMatrix()
{
	return glm::perspective(glm::radians(fov), (float)*width / (float)*height, closePlane, farPlane);
}

void FirstPersonCamera::moveUp(float speed)
{
	position += upPositipon * speed * this->speed;
}

void FirstPersonCamera::moveDown(float speed)
{
	moveUp(-speed);
}

void FirstPersonCamera::moveLeft(float speed)
{
	moveRight(-speed);
}

void FirstPersonCamera::moveRight(float speed)
{
	position += glm::normalize(glm::cross(viewDirection, upPositipon)) * speed * this->speed;
}

void FirstPersonCamera::moveFront(float speed)
{
	position += viewDirection * speed * this->speed;
}

void FirstPersonCamera::moveBack(float speed)
{
	moveFront(-speed);
}

void FirstPersonCamera::updatePosition(glm::vec2 delta)
{

	glm::vec3 toRotate = glm::cross(viewDirection, upPositipon);
	glm::vec3 oldPos = viewDirection;
	glm::vec3 tiltTest = glm::mat3(glm::rotate(glm::radians(delta.y * rSpeed), toRotate)) * viewDirection;

	viewDirection = glm::mat3(glm::rotate(glm::radians(delta.x * rSpeed), upPositipon)) * viewDirection;

	/*
	constexpr float maxUpDown = 0.95;
	if (delta.y < 0)
	{	//down
		if (viewDirection.y < -maxUpDown)
		{
			//viewDirection.y = -maxUpDown;

			goto noMove;
		}
	}
	else
	{	//up
		if (viewDirection.y > maxUpDown)
		{
			//viewDirection.y = maxUpDown;
			goto noMove;
		}
	}

	viewDirection = glm::mat3(glm::rotate(glm::radians(delta.y * rSpeed), toRotate)) * viewDirection;
	viewDirection = glm::normalize(viewDirection);
	*/


	if (isPositive(oldPos.x) == !isPositive(tiltTest.x) &&
		isPositive(oldPos.z) == !isPositive(tiltTest.z))
	{

		// no move
	}
	else
	{
		glm::vec3 newDir = glm::mat3(glm::rotate(glm::radians(delta.y * rSpeed), toRotate)) * viewDirection;
		newDir = glm::normalize(newDir);
		viewDirection = newDir;
	}

	//todo remove last camera problems
	//llog((viewDirection.x), (viewDirection.y), (viewDirection.z));
}

float FirstPersonCamera::getTopDownAngle() const
{
	glm::vec2 dir = {viewDirection.x, viewDirection.z};
	dir = glm::normalize(dir);
	float angle = std::acosf(dir.x);
	if(dir.y <0)
	{
		angle = (3.14159 * 2) - angle;
	}

	return angle;
}

glm::mat4 FirstPersonCamera::getObjectToWorldMatrix()
{
	return glm::lookAt(position, position + viewDirection, upPositipon);
}
