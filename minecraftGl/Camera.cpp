/////////////////////////////////////////////
//Camera.cpp
//Copyright(c) 2019 Luta Vlad
//https://github.com/meemknight/OpenGLEngine
/////////////////////////////////////////////

#include "Camera.h"
#include "platformIndependentCalls.h"
//#include <Windows.h>

glm::vec3 Camera::getFirstPersonPosition()
{
	glm::vec3 pos = playerPosition;
	pos.y += distanceFromPlayer * sin(cameraAngle);
	float distanceProjection = distanceFromPlayer * cos(cameraAngle);
	pos.x += sin(topDownAngle) * distanceProjection;
	pos.z += cos(topDownAngle) * distanceProjection;
	return pos;
}

glm::vec3 Camera::getCurrentViewingPosition()
{
	if (firstPersonCamera)
	{
		return getFirstPersonPosition();
	}
	else
	{
		return position;
	}
}

Camera::Camera()
{
}

Camera::Camera(float angle, float closePlane, float farPlane, int * width, int * height)
{
	projectionData.angle = angle;
	projectionData.width = width;
	projectionData.height = height;
	projectionData.closePlane = closePlane;
	projectionData.farPlane = farPlane;
}
/*
void Camera::setAngle(float a)
{
	float x, y, z;
	y = 0;

	z = -cos(a);
	x = sin(a);

	viewDirection = { x,y,z };
	angle = a;
}

void Camera::move(glm::vec3 m)
{

	float lenght = sqrt(pow(m.x, 2) + pow(m.z, 2));
	if (lenght <= 0.001 && lenght >= -0.001)return;
	float a = asin(m.z / lenght); //?

	m.x = cos(a + angle)*lenght;
	m.z = sin(a + angle)*lenght;

	position += m;

}

*/

glm::mat4 Camera::getObjectToWorld()
{
	if (!firstPersonCamera)
	{
		return glm::lookAt(position, position + viewDirection, upPositipon);
	}
	else
	{
		return glm::lookAt(getFirstPersonPosition(), playerPosition, upPositipon);
	}
}

glm::mat4 Camera::getProjectionViewMatrix()
{
	return glm::perspective(glm::radians(projectionData.angle), (float)*projectionData.width / (float)*projectionData.height, projectionData.closePlane, projectionData.farPlane) * getObjectToWorld();
}

glm::mat4 Camera::getProjectionMatrix()
{
	return glm::perspective(glm::radians(projectionData.angle), (float)*projectionData.width / (float)*projectionData.height, projectionData.closePlane, projectionData.farPlane);
}

void Camera::mouseUpdate(const glm::vec2 & pos)
{

	glm::vec2 delta = pos - oldMousePosition;
	delta.x *= -1;
	//delta.y *= -1;

	//if (glm::length(delta) > 50.f) 
	//{
	//	oldMousePosition = pos;
	//	return;
	//}

	glm::vec3 toRotate = glm::cross(viewDirection, upPositipon);

	viewDirection = glm::mat3(glm::rotate(glm::radians(delta.x * rSpeed), upPositipon)) * viewDirection;

	if (delta.y > 0)
	{	//down
		if (viewDirection.y < -0.99)
			goto noMove;
	}
	else
	{	//up
		if (viewDirection.y > 0.99)
			goto noMove;
	}

	viewDirection = glm::mat3(glm::rotate(glm::radians(delta.y * rSpeed), toRotate)) * viewDirection;
noMove:

	oldMousePosition = pos;
}

/*
void Camera::move(const glm::vec3 & move)
{
#pragma error theFunctionIsNotImplemented //todo implement this
}
*/

void Camera::moveUp(float speed)
{
	position += speed * mSpeed * upPositipon;
}

void Camera::moveDown(float speed)
{
	position -= speed * mSpeed * upPositipon;
}

void Camera::moveLeft(float speed)
{
	position -= speed * mSpeed * glm::normalize(glm::cross(viewDirection, upPositipon));
}

void Camera::moveRight(float speed)
{
	position += speed * mSpeed * glm::normalize(glm::cross(viewDirection, upPositipon));
}

void Camera::moveFront(float speed)
{
	if (flyCamera)
	{
		position += speed * mSpeed * viewDirection;
	}
	else
	{
		position -= speed * mSpeed * glm::normalize(glm::cross(glm::cross(viewDirection, upPositipon), upPositipon));
	}

}

void Camera::moveBack(float speed)
{
	if (flyCamera)
	{
		position -= speed * mSpeed * viewDirection;
	}
	else
	{
		position += speed * mSpeed * glm::normalize(glm::cross(glm::cross(viewDirection, upPositipon), upPositipon));
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

void FirstPersonCamera::mouseUpdate(const glm::vec2 & pos)
{

	glm::vec2 delta = pos - oldMousePosition;
	delta.x *= -1;
	delta.y *= -1;

	//if (glm::length(delta) > 50.f) 
	//{
	//	oldMousePosition = pos;
	//	return;
	//}

	glm::vec3 toRotate = glm::cross(viewDirection, upPositipon);

	viewDirection = glm::mat3(glm::rotate(glm::radians(delta.x * rSpeed), upPositipon)) * viewDirection;

	if (delta.y < 0)
	{	//down
		if (viewDirection.y < -0.99)
			goto noMove;
	}
	else
	{	//up
		if (viewDirection.y > 0.99)
			goto noMove;
	}

	viewDirection = glm::mat3(glm::rotate(glm::radians(delta.y * rSpeed), toRotate)) * viewDirection;
noMove:
	setRelMousePosition(getWindowSizeX() / 2, getWindowSizeY() / 2);
	oldMousePosition = getRelMousePosition();
}

glm::mat4 FirstPersonCamera::getObjectToWorldMatrix()
{
	return glm::lookAt(position, position + viewDirection, upPositipon);
}
