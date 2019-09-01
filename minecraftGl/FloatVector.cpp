#include "FloatVector.h"
#include <cstringt.h>

void FloatVector::reserve(int size)
{
	if(capacity < size)
	{
		float *tempData = new float[size];
		memcpy(tempData, data, this->size);
		delete[] data;
		data = tempData;
		capacity = size;
	}
}

void FloatVector::push(float x)
{
	size++;
	if(size >= capacity)
	{
		reserve(capacity * 2 + 1);
	}
	data[size - 1] = x;
}

void FloatVector::push(float x, float y)
{
	size+=2;
	if (size >= capacity)
	{
		reserve(capacity * 2 + 2);
	}
	data[size - 2] = x;
	data[size - 1] = y;
}

void FloatVector::push(float x, float y, float z)
{
	size += 3;
	if (size >= capacity)
	{
		reserve(capacity * 2 + 3);
	}
	data[size - 3] = x;
	data[size - 2] = y;
	data[size - 1] = z;
}

void FloatVector::cleanup()
{
	if(data)
	{
		delete[] data;
	}
}
