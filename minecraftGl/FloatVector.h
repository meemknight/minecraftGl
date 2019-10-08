#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct FloatVector
{
	float *data = 0;
	int size = 0;
	int capacity = 0;

	void reserve(int size);
	void push(float x);
	void push(float x, float y);
	void push(float x, float y, float z);
	void push(glm::vec3 v) { push(v.x, v.y, v.z); }
	void push(glm::vec4 v) { push(v.x, v.y, v.z); push(v.w); }//todo
	void cleanup();
};

