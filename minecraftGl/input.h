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
			breakBlock,
			placeBlock,
			q,
			e,
			buttonsCount
		};
	};

	void loadXinput();
	glm::vec2 getMoveDirection();
	glm::vec2 getLookDirection();
	bool isKeyPressedOn(int b);
	bool isKeyHeld(int b);

	//used internally
	namespace internal
	{
		bool getisKeyHeldDirect(int b, const XINPUT_STATE *s);
	}

	void updateInput();

}