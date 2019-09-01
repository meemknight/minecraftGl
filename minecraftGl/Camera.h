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

class Camera
{
public:

	glm::vec3 viewDirection = { 0.f, 0.f, -1.f };
	glm::vec3 upPositipon = { 0, 1, 0 };
	glm::vec3 position = { 2,0,2 };

	glm::vec3 playerPosition = { 0,0,0 };
	///this distance is only on the x and z axes
	float distanceFromPlayer = 0;
	float cameraAngle = 0;
	float topDownAngle = 0;

public:
	glm::vec2 oldMousePosition;
	float rSpeed = 0.2f;
	float mSpeed = 0.05f;

	bool flyCamera = 1;
	bool firstPersonCamera = 0;

	glm::vec3 getFirstPersonPosition();

	///this function will get the right position taking into acount
	///the type of the camera (first person or free camera)
	glm::vec3 getCurrentViewingPosition();

	struct
	{
		float angle; ///angle of the view projecton in angles not radians
		int *width; /// pointer to the idth of the screen
		int *height; /// pointer to the height of the screen
		float closePlane;
		float farPlane;
	}projectionData;

	Camera();
	Camera(float angle, float closePlane, float farPlane, int *width, int *height);


	glm::mat4 getObjectToWorld(); ///gets the object to world for the camera
	glm::mat4 getProjectionViewMatrix(); /// gets the full view transform
	glm::mat4 getProjectionMatrix();


	///used for mouse moving
	void mouseUpdate(const glm::vec2 &pos);
	
	//void move(const glm::vec3 &move);
	void moveUp(float speed);
	void moveDown(float speed);
	void moveLeft(float speed);
	void moveRight(float speed);
	void moveFront(float speed);
	void moveBack(float speed);

};

struct FirstPersonCamera
{
	FirstPersonCamera() {};
	FirstPersonCamera(float fov, float closePlane, float farPlane, int *width, int *height);

	glm::vec3 viewDirection = { 0.f, 0.f, -1.f };
	glm::vec3 upPositipon = { 0, 1, 0 };
	glm::vec3 position = { 0,0,4 };

	float fov;
	float closePlane;
	float farPlane;
	int *width; /// pointer to the idth of the screen
	int *height; /// pointer to the height of the screen

	glm::mat4 getProjectionViewMatrix(); /// gets the full view transform
	glm::mat4 getObjectToWorldMatrix(); ///gets the object to world for the camera
	glm::mat4 getProjectionMatrix();

	float speed = 5;

	void moveUp(float speed);
	void moveDown(float speed);
	void moveLeft(float speed);
	void moveRight(float speed);
	void moveFront(float speed);
	void moveBack(float speed);

	void mouseUpdate(const glm::vec2 &pos);

	float rSpeed = 0.3f;

private:
	glm::vec2 oldMousePosition = {};
};