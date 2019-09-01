#pragma once
struct FloatVector
{
	float *data = 0;
	int size = 0;
	int capacity = 0;

	void reserve(int size);
	void push(float x);
	void push(float x, float y);
	void push(float x, float y, float z);
	void cleanup();
};

