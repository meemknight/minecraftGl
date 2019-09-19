#include "FloatVector.h"
#include <cstringt.h>

void FloatVector::reserve(int size)
{
	if(capacity < size)
	{
		float *tempData = new float[size];
		memcpy(tempData, data, this->size * sizeof(float));

		delete[] data;
		data = tempData;
		capacity = size;
	}
}

void FloatVector::push(float x)
{
	if(size + 1 >= capacity)
	{
		reserve(capacity * 2 + 1);
	}
	size++;

	data[size - 1] = x;
}

void FloatVector::push(float x, float y)
{
	if (size + 2 >= capacity)
	{
		reserve(capacity * 2 + 2);

	}
	size += 2;

	data[size - 2] = x;
	data[size - 1] = y;
}

void FloatVector::push(float x, float y, float z)
{
	if (size + 3 >= capacity)
	{
		reserve(capacity * 2 + 3);
	}
	
	size += 3;

	data[size - 3] = x;
	data[size - 2] = y;
	data[size - 1] = z;
}

void FloatVector::cleanup()
{
	if(data)
	{
		delete[] data;
		size = 0;
		capacity = 0;
	}
}
