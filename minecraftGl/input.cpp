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
			xInputLoaded = 1;
		}

	}

	int bindings[Buttons::buttonsCount] = { 0, 'W', 'S', 'A', 'D', VK_SPACE };

	glm::vec2 getMoveDirection()
	{
		glm::vec2 dir = {};

		if(isKeyPressed(bindings[Buttons::up]))
		{
			dir.x += 1;
		}
		if (isKeyPressed(bindings[Buttons::down]))
		{
			dir.x -= 1;
		}
		if (isKeyPressed(bindings[Buttons::left]))
		{
			dir.y -= 1;
		}
		if (isKeyPressed(bindings[Buttons::right]))
		{
			dir.y += 1;
		}

		if(xInputLoaded)
		{
			XINPUT_STATE s;

			if (DynamicXinputGetState != nullptr && DynamicXinputGetState(0, &s) == ERROR_SUCCESS)
			{
				XINPUT_GAMEPAD *pad = &s.Gamepad;

				float retValX = pad->sThumbLX / (float)SHRT_MAX;

				retValX = max(-1.f, retValX);
				retValX = min(1.f, retValX);

				if (abs(retValX) < 0.12)
				{
					retValX = 0.f;
				}

				float retValY = pad->sThumbLY / (float)SHRT_MAX;

				retValY = max(-1.f, retValY);
				retValY = min(1.f, retValY);

				if (abs(retValY) < 0.12)
				{
					retValY = 0.f;
				}

				dir += glm::vec2{ retValY, retValX };

				dir = glm::clamp(dir, { -1,-1 }, { 1,1 });
			}
		
		}

		return dir;
	}

	
};
