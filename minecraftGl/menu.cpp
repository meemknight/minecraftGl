#include "menu.h"
#include <vector>
#include "Ui.h"
#include "tools.h"
#include "input.h"
#include <iomanip>

struct lineInfo
{
	enum type
	{
		uninteractableCentreText,
		booleanTextBox,
		slider0_1,
		interactableText,
	};

	int type = 0;
	const char* text;
	bool *bVal;
	float *fVal;
};

struct
{
	int cursorIndex = 0;
	int lastcursorIndex = 0;

}perMenuData;

struct
{
	std::vector <lineInfo> lines;

}perFrameData;


void menu::startMenu()
{
	perFrameData = {};
}

void menu::resetMenuState()
{
	perMenuData = {};
}

void menu::uninteractableCentreText(const char * text)
{
	lineInfo info = {};
	info.type = info.uninteractableCentreText;
	info.text = text;
	perFrameData.lines.push_back(info);
}

void menu::booleanTextBox(const char * text, bool * b)
{
	lineInfo info = {};
	info.type = info.booleanTextBox;
	info.text = text;
	info.bVal = b;
	perFrameData.lines.push_back(info);

}

void menu::slider0_1(const char * text, float *val)
{
	lineInfo info = {};
	info.type = info.slider0_1;
	info.text = text;
	info.fVal = val;
	perFrameData.lines.push_back(info);
}

void menu::interactableText(const char * text, bool *hasPressed)
{
	lineInfo info = {};
	info.type = info.interactableText;
	info.text = text;
	info.bVal = hasPressed;
	perFrameData.lines.push_back(info);
}

float speed = 0.3;

void menu::endMenu(gl2d::Renderer2D & renderer, gl2d::Texture backgroundTexture, gl2d::Font f, bool * backPressed, float deltaTime)
{
	//input bindings
	///todo add your own functions here

	bool usingControllerInput = input::isControllerInput();
	bool acceptKeyReleased = input::isKeyReleased(input::Buttons::jump);
	bool leftPressed = input::isKeyHeld(input::Buttons::left);
	bool rightPressed = input::isKeyHeld(input::Buttons::right);
	bool upReleased = input::isKeyReleased(input::Buttons::up);
	bool downReleased = input::isKeyReleased(input::Buttons::down);
	bool escReleased = input::isKeyReleased(input::Buttons::esc);

	//


	auto c = renderer.currentCamera;
	renderer.currentCamera.setDefault();


	float inPerc = 0.9;

	renderer.renderRectangle({ 0,0,renderer.windowW, renderer.windowH }, {}, 0,
		backgroundTexture);

#pragma region setInnerRegion
	glm::vec2 inPercXY = { renderer.windowW, renderer.windowH };
	inPercXY -= glm::vec2(renderer.windowW, renderer.windowH) * inPerc;

	Ui::Frame frame({ inPercXY.x, inPercXY.y, renderer.windowW * inPerc, renderer.windowH * inPerc });
#pragma endregion

	float posy = 20;


	int count = 0;
	for (auto &i : perFrameData.lines)
	{
		bool isSelected = false;
		if (perMenuData.cursorIndex == count)
		{
			isSelected = true;
		}

		glm::vec4 textBox = Ui::Box().xLeft(20).yTop(posy);
		glm::vec2 size = renderer.getTextSize(i.text, f, 0.7);
		textBox.z = size.x;
		textBox.w = size.y;
		auto p = getRelMousePosition();

		bool bmouseMoved = mouseMoved();

		switch (i.type)
		{
		case i.uninteractableCentreText:
		{
			glm::vec4 center = Ui::Box().xCenter().yCenter()();
			renderer.renderText({ textBox.x, textBox.y }, i.text, f, { 0.4,0.4,0.4,1 },
				0.7, 4, 3, false, { 0.1,0.1,0.1,1 }, { 0.2,0.3,0.5,0.7 });
		}
		break;
		case i.booleanTextBox:
		{
			float bonusW = renderer.getTextSize(" : off ", f, 0.7, 4, 3).x + 20;

			glm::vec4 color = { 1,1,1,1 };
			glm::vec4 box = textBox;
			box.y -= box.w;
			box.z += bonusW;

			if (!usingControllerInput)
			{
				if (Ui::isInButton(p, box) && bmouseMoved)
				{
					isSelected = true;
					perMenuData.cursorIndex = count;
				}
			}

			if (isSelected)
			{
				color = { 1,1,0.2,1 };
				textBox.x += 20;
				renderer.renderText({ textBox.x, textBox.y }, i.text, f, color,
					0.7, 4, 3, false);

				if (acceptKeyReleased ||
					(!usingControllerInput
						&&
						Ui::isButtonReleased(p, box)
						)
					)
				{
					if (i.bVal)
						*i.bVal = !*i.bVal;
				}
			}
			else
			{
				renderer.renderText({ textBox.x, textBox.y }, i.text, f, color,
					0.7, 4, 3, false);
			}


			if (i.bVal)
			{
				const char *c;
				if (*i.bVal)
				{
					c = " : on";
				}
				else
				{
					c = " : off";
				}
				renderer.renderText({ textBox.x + textBox.z, textBox.y }, c, f, color,
					0.7, 4, 3, false);
			}



			count++;
		}
		break;
		case i.slider0_1:
		{
			float bonusW = renderer.getTextSize(" : 100% ", f, 0.7, 4, 3).x + 20;

			glm::vec4 box = textBox;
			box.y -= box.w;
			box.z += bonusW;

			if (!usingControllerInput)
			{
				if (Ui::isInButton(p, box) && bmouseMoved)
				{
					isSelected = true;
					perMenuData.cursorIndex = count;
				}
			}

			glm::vec4 color(1, 1, 1, 1);

			if (isSelected)
			{
				color = { 1,1,0.2,1 };
				textBox.x += 20;
				renderer.renderText({ textBox.x, textBox.y }, i.text, f, color,
					0.7, 4, 3, false, { 0.1,0.1,0.1,1 });

				if (i.fVal)
				{
					if (leftPressed)
					{
						*i.fVal -= deltaTime * speed;
					}
					if (rightPressed)
					{
						*i.fVal += deltaTime * speed;
					}

					if (*i.fVal < 0) { *i.fVal = 0; }
					if (*i.fVal > 1) { *i.fVal = 1; }
				}

			}
			else
			{
				renderer.renderText({ textBox.x, textBox.y }, i.text, f, color,
					0.7, 4, 3, false, { 0.1,0.1,0.1,1 });
			}

			float v = *i.fVal * 100;

			int precision = 1;
			if (v == 0 || v == 100)
			{
				precision = 0;
			}

			std::stringstream stream;
			stream << std::fixed << std::setprecision(precision) << v;
			std::string s = stream.str();

			renderer.renderText({ textBox.x + textBox.z, textBox.y }, (" : " + stream.str() + "%").c_str(), f, color,
				0.7, 4, 3, false, { 0.1,0.1,0.1,1 });
			count++;
		}
		break;
		case i.interactableText:
		{
			float bonusW = renderer.getTextSize(" ", f, 0.7, 4, 3).x + 20;

			glm::vec4 box = textBox;
			box.y -= box.w;
			box.z += bonusW;

			if (!usingControllerInput)
			{
				if (Ui::isInButton(p, box) && bmouseMoved)
				{
					isSelected = true;
					perMenuData.cursorIndex = count;
				}
			}

			if (isSelected)
			{
				textBox.x += 20;
				renderer.renderText({ textBox.x, textBox.y }, i.text, f, { 1,1,0.2,1 },
					0.7, 4, 3, false, { 0.1,0.1,0.1,1 });

				if (acceptKeyReleased ||
					(
						!usingControllerInput
						&&
						Ui::isButtonReleased(p, box)
						)
					)
				{
					if (i.bVal)
						*i.bVal = true;
				}
			}
			else
			{
				renderer.renderText({ textBox.x, textBox.y }, i.text, f, { 1,1,1,1 },
					0.7, 4, 3, false, { 0.1,0.1,0.1,1 });
			}

			count++;
		}
		break;
		default:
			break;
		}

		posy += 20 + textBox.w;

	}


	if (upReleased)
	{
		perMenuData.cursorIndex--;
	}

	if (downReleased)
	{
		perMenuData.cursorIndex++;
	}

	if (escReleased)
	{
		if (backPressed)
		{
			*backPressed = true;
		}
	}

	if (perMenuData.cursorIndex < 0)
	{
		perMenuData.cursorIndex = count - 1;
	}
	else
		if (perMenuData.cursorIndex >= count)
		{
			perMenuData.cursorIndex = 0;
		}

	renderer.currentCamera = c;

}
