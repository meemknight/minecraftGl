/////////////////////////////////////////////
//Camera.cpp
//Copyright(c) 2019 Luta Vlad
//https://github.com/meemknight/OpenGLEngine
/////////////////////////////////////////////

#include "Camera.h"
#include "platformIndependentCalls.h"
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
	
	/*
	glm::vec2 min = {}, max = {};
	
	ExtractPlanesGL(getProjectionViewMatrix(),min, max);

	min /= CHUNK_SIZE;
	max /= CHUNK_SIZE;

	if(min.x > max.x)
	{
		std::swap(min.x, max.x);
	}

	if (min.y > max.y)
	{
		std::swap(min.y, max.y);
	}

	min.x = floorf(min.x);
	min.y = floorf(min.y);
	max.x = floorf(max.x);
	max.y = floorf(max.y);

	min--;
	max++;
		
	for (int y = min.y; y <= max.y; y++)
	{
		for (int x = min.x; x <= max.x; x++)
		{
			chunksToLoad.emplace_back(x, 0, y);
		}
	}
	*/

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

void FirstPersonCamera::mouseUpdate(const glm::vec2 & pos)
{
	glm::vec2 delta = pos - oldMousePosition;
	delta.x *= -1;
	delta.y *= -1;


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
	}else
	{
		glm::vec3 newDir = glm::mat3(glm::rotate(glm::radians(delta.y * rSpeed), toRotate)) * viewDirection;
		newDir = glm::normalize(newDir);
		viewDirection = newDir;
	}

	//todo remove last camera problems
	//llog((viewDirection.x), (viewDirection.y), (viewDirection.z));

	setRelMousePosition(getWindowSizeX() / 2, getWindowSizeY() / 2);
	oldMousePosition = getRelMousePosition();
}

float FirstPersonCamera::getTopDownAngle()
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
