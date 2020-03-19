#include "Ui.h"

namespace Ui
{
	static int xPadd = 0;
	static int yPadd = 0;
	static int width = 0;
	static int height = 0;

	Frame::Frame(glm::ivec4 size)
	{
		this->loaded = 1;
		lastW = width;
		lastH = height;
		lastX = xPadd;
		lastY = yPadd;

		width = size.z;
		height = size.w;
		xPadd = size.x;
		yPadd = size.y;
	}

	Frame::~Frame()
	{
		if(loaded)
		{
			width = lastW;
			height = lastH;
			xPadd = lastX;
			yPadd = lastY;
		}
	}

	glm::ivec4 Box::operator()()
	{

		if(XcenterState == -1)
		{
			dimensions.x = xPadd;
		}
		if (YcenterState == -1)
		{
			dimensions.y = yPadd;
		}
		//todo handle other cases

		dimensions.z = 100;
		dimensions.w = 100;

		return dimensions;
	}

	Box & Box::xDistancePixels(int dist)
	{
		dimensions.x = dist;
		XcenterState = 0;
		return *this;
	}

	Box & Box::yDistancePixels(int dist)
	{
		dimensions.y = dist;
		YcenterState = 0;
		return *this;
	}

	Box & Box::xCenter()
	{
		XcenterState = 1;
		return *this;
	}
	Box & Box::yCenter()
	{
		YcenterState = 1;
		return *this;
	}
	Box & Box::xLeft()
	{
		XcenterState = -1;
		return *this;
	}
	Box & Box::yTop()
	{
		YcenterState = -1;
		return *this;
	}
	Box & Box::xRight()
	{
		XcenterState = 2;
		return *this;
	}
	Box & Box::yBottom()
	{
		YcenterState = 2;
		return *this;
	}
}

