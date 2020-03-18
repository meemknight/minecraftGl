#include "input.h"
#include <Windows.h>
#include"platformIndependentCalls.h"
#include <math.h>

namespace input
{

	float max(float a, float b)
	{
		return a > b ? a : b;
	}

	float min(float a, float b)
	{
		return a < b ? a : b;
	}

	typedef DWORD WINAPI XInputGetState_t(DWORD dwUserIndex, XINPUT_STATE* pState);
	static XInputGetState_t *DynamicXinputGetState;
	typedef DWORD WINAPI XInputSetState_t(DWORD dwUserIndex, XINPUT_VIBRATION* pState);
	static XInputSetState_t *DynamicXinputSetState;
	typedef DWORD WINAPI XInputGetKeystroke_t(DWORD dwUserIndex, DWORD dwReserved, PXINPUT_KEYSTROKE pKeystroke);
	XInputGetKeystroke_t *DynamicXInputGetKeystroke;
 
	static bool xInputLoaded = 0;

	void loadXinput()
	{

		HMODULE xinputLib = LoadLibrary("xinput1_4.dll");
		if (!xinputLib)
		{
			xinputLib = LoadLibrary("xinput1_3.dll");
		}
		if (!xinputLib)
		{
			return;
		}

		{
			DynamicXinputGetState = (XInputGetState_t*)GetProcAddress(xinputLib, "XInputGetState");
			DynamicXinputSetState = (XInputSetState_t*)GetProcAddress(xinputLib, "XInputSetState");
			DynamicXInputGetKeystroke = (XInputGetKeystroke_t*)GetProcAddress(xinputLib, "XInputGetKeystroke");
			xInputLoaded = 1;
		}

	}

	int bindings[Buttons::buttonsCount] = { 0, 'W', 'S', 'A', 'D', VK_SPACE, 0, 0 };
	WORD bindingsController[Buttons::buttonsCount] = { 0, 0, 0, 0, 0, XINPUT_GAMEPAD_RIGHT_THUMB, 0, 0};
	float deadZone = 0.15;

	int buttonsHeld[Buttons::buttonsCount] = {};
	int buttonsPressed[Buttons::buttonsCount] = {};

	glm::vec2 getLookDirection()
	{
		glm::vec2 dir = {};

		if (xInputLoaded)
		{
			XINPUT_STATE s;

			if (xInputLoaded != 0 && DynamicXinputGetState(0, &s) == ERROR_SUCCESS)
			{
				XINPUT_GAMEPAD *pad = &s.Gamepad;

				float retValX = pad->sThumbRX / (float)SHRT_MAX;

				retValX = max(-1.f, retValX);
				retValX = min(1.f, retValX);

				if (abs(retValX) < deadZone)
				{
					retValX = 0.f;
				}

				float retValY = pad->sThumbRY / (float)SHRT_MAX;

				retValY = max(-1.f, retValY);
				retValY = min(1.f, retValY);

				if (abs(retValY) < deadZone)
				{
					retValY = 0.f;
				}

				dir += glm::vec2{ -retValX, retValY };

				dir = glm::clamp(dir, { -1,-1 }, { 1,1 });
			}

		}

		return dir;
	}

	bool isKeyPressedOn(int b)
	{
		return buttonsPressed[b];
	}

	glm::vec2 getMoveDirection()
	{
		glm::vec2 dir = {};

		if(::isKeyPressed(bindings[Buttons::up]))
		{
			dir.x += 1;
		}
		if (::isKeyPressed(bindings[Buttons::down]))
		{
			dir.x -= 1;
		}
		if (::isKeyPressed(bindings[Buttons::left]))
		{
			dir.y -= 1;
		}
		if (::isKeyPressed(bindings[Buttons::right]))
		{
			dir.y += 1;
		}

		if(xInputLoaded)
		{
			XINPUT_STATE s;

			if (xInputLoaded != 0 && DynamicXinputGetState(0, &s) == ERROR_SUCCESS)
			{
				XINPUT_GAMEPAD *pad = &s.Gamepad;

				float retValX = pad->sThumbLX / (float)SHRT_MAX;

				retValX = max(-1.f, retValX);
				retValX = min(1.f, retValX);

				if (abs(retValX) < deadZone)
				{
					retValX = 0.f;
				}

				float retValY = pad->sThumbLY / (float)SHRT_MAX;

				retValY = max(-1.f, retValY);
				retValY = min(1.f, retValY);

				if (abs(retValY) < deadZone)
				{
					retValY = 0.f;
				}

				dir += glm::vec2{ retValY, retValX };

				dir = glm::clamp(dir, { -1,-1 }, { 1,1 });
			}
		
		}

		return dir;
	}

	//bool isKeyPressedOn(int b)
	//{
	//	bool val = 0;
	//
	//	XINPUT_KEYSTROKE info;
	//
	//	if(xInputLoaded != 0 && DynamicXInputGetKeystroke(0, 0, &info) == ERROR_SUCCESS)
	//	{
	//		if ((info.VirtualKey & VK_PAD_B) && (info.Flags & XINPUT_KEYSTROKE_KEYDOWN))
	//		{
	//			val = 1;
	//		}
	//	}
	//
	//	val |= ::isKeyPressedOn(bindings[b]);
	//	return val;
	//}

	bool isKeyHeld(int b)
	{
		return buttonsHeld[b];
	}

	namespace internal
	{
		bool getisKeyHeldDirect(int b, const XINPUT_STATE *s)
		{

			bool val = 0;
			
			if(s)
			{
				const XINPUT_GAMEPAD *pad = &s->Gamepad;

				if (b == input::Buttons::placeBlock || b == input::Buttons::breakBlock)
				{
					if (b == input::Buttons::placeBlock)
					{
						val = pad->bRightTrigger > 10;
					}
					else
					{
						val = pad->bLeftTrigger > 10;
					}
				}
				else
				{
					val = (pad->wButtons & bindingsController[b]);
				}

			}

			if (b == input::Buttons::placeBlock || b == input::Buttons::breakBlock)
			{
				if (b == input::Buttons::placeBlock)
				{
					val |= ::isRMouseButtonPressed();
				}
				else
				{
					val |= ::isLMouseButtonPressed();
				}
			}
			else
			{
				val |= ::isKeyPressed(bindings[b]);
			}

			return val;

		}
	}

	void updateInput()
	{
		XINPUT_STATE s;
		bool read = 1;
		if (xInputLoaded == 0 || DynamicXinputGetState(0, &s) != ERROR_SUCCESS)
		{
			read = 0;
		}

		for(int i=0; i<Buttons::buttonsCount; i++)
		{
			bool state;
			if(read == 1)
			{
				state = internal::getisKeyHeldDirect(i, &s);
			}else
			{
				state = internal::getisKeyHeldDirect(i, nullptr);
			}

			buttonsPressed[i] = 0;

			if(state)
			{
				if (buttonsHeld[i] == 0)
				{
					buttonsPressed[i] = 1;
				}
			}
			buttonsHeld[i] = state;

		}

	}

	
};
