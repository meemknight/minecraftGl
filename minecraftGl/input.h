#pragma once
#include <Windows.h>
#include <Xinput.h>
#include <glm/vec2.hpp>

namespace input
{

	namespace Buttons
	{
		enum
		{
			none = 0,
			up,
			down,
			left,
			right,
			jump,
			buttonsCount
		};
	};

	void loadXinput();
	glm::vec2 getMoveDirection();
	glm::vec2 getLookDirection();
	bool isKeyPressedOn(int b);
	bool isKeyHeld(int b);

}