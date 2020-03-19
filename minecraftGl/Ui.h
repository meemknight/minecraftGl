#pragma once
#include "glm/vec4.hpp"

namespace Ui
{
	class Frame
	{
		int lastW;
		int lastH;
		int lastX;
		int lastY;

		bool loaded = 0;
	
	public:
	
		Frame(const Frame &other) = delete;
		Frame(const Frame &&other) = delete;
		Frame(glm::ivec4 size);
		~Frame();
	};

	struct Box
	{
		glm::ivec4 dimensions = {};

		//-1 left
		// 0 none
		// 1 center
		// 2 right
		// 3 reversePixels
		unsigned char XcenterState = 0;
		unsigned char YcenterState = 0;

		Box &xDistancePixels(int dist);
		Box &yDistancePixels(int dist);
		Box &xCenter();
		Box &yCenter();
		Box &xLeft();
		Box &yTop();
		Box &xRight();
		Box &yBottom();

		glm::ivec4 operator()();
	};


};

