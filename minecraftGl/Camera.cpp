/////////////////////////////////////////////
//Camera.cpp
//Copyright(c) 2019 Luta Vlad
//https://github.com/meemknight/OpenGLEngine
/////////////////////////////////////////////

#include "Camera.h"
#include <Windows.h>

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

Camera::Camera(HWND hand, float angle, float * width, float * height, float closePlane, float farPlane)
{
	window = hand;
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
	return glm::perspective(glm::radians(projectionData.angle), *projectionData.width / *projectionData.height, projectionData.closePlane, projectionData.farPlane) * getObjectToWorld();
}

glm::mat4 Camera::getProjectionMatrix()
{
	return glm::perspective(glm::radians(projectionData.angle), *projectionData.width / *projectionData.height, projectionData.closePlane, projectionData.farPlane);
}

void Camera::mouseUpdate(const glm::vec2 & pos)
{
	glm::vec2 delta = pos - oldMousePosition;

	//if (glm::length(delta) > 50.f) 
	//{
	//	oldMousePosition = pos;
	//	return;
	//}

	glm::vec3 toRotate = glm::cross(viewDirection, upPositipon);

	viewDirection = glm::mat3(glm::rotate(glm::radians(-delta.x * rSpeed), upPositipon)) * viewDirection;

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

	viewDirection = glm::mat3(glm::rotate(glm::radians(-delta.y * rSpeed), toRotate)) * viewDirection;
noMove:


	oldMousePosition = pos;
	
	RECT rect;
	GetWindowRect(window, &rect);

	//SetCursorPos(rect.left + ((rect.right - rect.left) / 2), rect.top + ((rect.bottom - rect.top) / 2));
	oldMousePosition = pos;
	//printf("%+f6 %+f6 %+f6 ", position.x, position.y, position.z);
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

