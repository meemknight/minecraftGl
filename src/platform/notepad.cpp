#define NOMINMAX
#include <notepad.h>
#include <Windows.h>
#include <string>
#include <iostream>
#include <memoryStuff.h>

#define WINDOW_NAME "GEAM IN NOTEPAD";

DWORD notepadPid;
HANDLE notepadHandle;
HWND editWindow;
HWND notepadWindow;
std::vector<void *>notepadFramebufferMemory;
char *frameBuffer;
RECT frameBufferSizeRect = {};


struct Size
{
	int x;
	int y;
	int time;

	int windowSizeX;
	int windowSizeY;
}sizes[3] = {{60, 20, 5000, 520, 450}, {80, 30, 5000, 680, 630}};

constexpr int CURRENT_SIZE = 1;

int sizeX = sizes[CURRENT_SIZE].x;
int sizeY = sizes[CURRENT_SIZE].y;
int windowSizeX = sizes[CURRENT_SIZE].windowSizeX;
int windowSizeY = sizes[CURRENT_SIZE].windowSizeY;
int sleepTime = sizes[CURRENT_SIZE].time;

//int sizeX = 80;
//int sizeY = 28;

size_t frameBufferSize = (sizeX + 2) * sizeY * 2;

//http://kylehalladay.com/blog/2020/05/20/Rendering-With-Notepad.html
HWND GetWindowForProcessAndClassName(DWORD pid, const char *className)
{
	HWND curWnd = GetTopWindow(0); //0 arg means to get the window at the top of the Z order
	char classNameBuf[256] = {};

	while (curWnd != NULL)
	{
		DWORD curPid;
		DWORD dwThreadId = GetWindowThreadProcessId(curWnd, &curPid);

		if (curPid == pid)
		{
			GetClassNameA(curWnd, classNameBuf, 256);
			if (strcmp(className, classNameBuf) == 0) return curWnd;

			HWND childWindow = FindWindowExA(curWnd, NULL, className, NULL);
			if (childWindow != NULL) return childWindow;
		}
		curWnd = GetNextWindow(curWnd, GW_HWNDNEXT);
	}
	return NULL;
}

//http://kylehalladay.com/blog/2020/05/20/Rendering-With-Notepad.html
bool writeStringToNotepadBruteAndFindPointer() 
{
	srand((unsigned)notepadPid + (unsigned)time(0));

	assert(notepadWindow);
	MoveWindow(notepadWindow, 100, 100, windowSizeX, windowSizeY, true);
	GetClientRect(notepadWindow, &frameBufferSizeRect);


	frameBuffer = (char *)malloc(frameBufferSize);

	int i = 0;
	for (int y = 0; y < sizeY; y++)
	{
		for (int x = 0; x < sizeX; x++)
		{
			char v = 0;
			if (i < 10)
			{
				const char *patern = "TOTALLYUNIQUE";
				v = patern[i];
			}
			else
			{
				v = 'A' + (rand() % 26);
			}

			PostMessage(editWindow, WM_CHAR, v, 0);
			frameBuffer[i * 2] = v;
			frameBuffer[i * 2 + 1] = 0x00;
			i++;
		}
		
		if (y != sizeY - 1)
		{
			PostMessage(editWindow, WM_CHAR, '\r', 0);
			frameBuffer[i * 2] = '\r';
			frameBuffer[i * 2 + 1] = 0x00;
			i++;

			frameBuffer[i * 2] = 10; //line feed
			frameBuffer[i * 2 + 1] = 0x00;
			i++;
		}
		else
		{
			frameBuffer[i * 2] = 0;
			frameBuffer[i * 2 + 1] = 0;
			i++;
			frameBuffer[i * 2] = 0; 
			frameBuffer[i * 2 + 1] = 0;
			i++;
		}

	}
	
	Sleep(sleepTime); //wait for input messages to finish processing...it's slow. 
	//Now use the frameBuffer as the unique byte pattern to search for

#pragma region block input
	if (1)
	{
		//auto style = GetWindowLongPtrA(notepadWindow, GWL_EXSTYLE);
		//style |= WS_EX_NOACTIVATE;
		//style |= WS_EX_APPWINDOW;
		//SetWindowLongPtrA(notepadWindow, GWL_EXSTYLE, style);

		auto style = GetWindowLongPtrA(editWindow, GWL_STYLE);
		style |= WS_DISABLED;
		SetWindowLongPtrA(editWindow, GWL_STYLE, style);

		//style = GetWindowLongPtrA(notepadWindow, GWL_STYLE);
		//style &= !WS_SIZEBOX;
		//SetWindowLongPtrA(notepadWindow, GWL_STYLE, style);
	}
#pragma endregion

	auto rez = findBytePatternInProcessMemory(notepadHandle, frameBuffer, std::min(frameBufferSize, (size_t)79));
	
	if (rez.empty())
	{
		std::cout << "couldn't get the notepad memory buffer\n";
		return 0;
	}else
	if (rez.size() <= 5)
	{
		notepadFramebufferMemory = rez;
	}
	else
	{
		std::cout << "couldn't get the notepad memory buffer, multiple pointers found\n";
		return 0;
	}

	return 1;
}

BOOL CALLBACK EnumWindowsProc(
	HWND   hwnd,
	LPARAM lParam
)
{
	DWORD pid = 0;
	GetWindowThreadProcessId(hwnd, &pid);

	if (pid == notepadPid)
	{
		notepadWindow = hwnd;
		return 0;
	}
	return true;
}

//http://kylehalladay.com/blog/2020/05/20/Rendering-With-Notepad.html
void UpdateText()
{
	
	SIZE_T written = 0;
	for (auto &add : notepadFramebufferMemory)
	{
		WriteProcessMemory(notepadHandle, add, frameBuffer, frameBufferSize-4, &written);
	}

	RECT r;
	GetClientRect(editWindow, &r);
	InvalidateRect(editWindow, &r, false);
}


char windowName[256] = WINDOW_NAME;
bool openTheNotepadInstance()
{

#pragma region open notepad
	PROCESS_INFORMATION processInformation = {};
	{
		STARTUPINFOA startupInfo = {sizeof(STARTUPINFOA)};
		startupInfo.lpTitle = windowName;

		char data[256] = "notepad.exe";
		if (!CreateProcessA(nullptr, data, NULL, NULL, FALSE, 0, NULL,
			NULL, &startupInfo, &processInformation))
		{
			std::cout << getLastErrorString() << "\n";
			return 0;
		}
	}
#pragma endregion

#pragma region get pid
	Sleep(5);
	notepadHandle = processInformation.hProcess;
	notepadPid = GetProcessId(processInformation.hProcess);

	if (!notepadPid)
	{
		std::cout << getLastErrorString() << "\n";
		return 0;
	}
#pragma endregion

#pragma region find the notepad window
	{
		int counter = 0;
		while (true)
		{
			EnumWindows(EnumWindowsProc, 0);

			if (!notepadWindow)
			{
				Sleep(5);
				counter++;

				if (counter > 150)
				{
					std::cout << "couldn't find the notepad window\n";
					return 0;
				}
			}
			else
			{
				break;
			}
		}
	}
#pragma endregion

	for (int i = 0; i < 100; i++)
	{
		editWindow = GetWindowForProcessAndClassName(notepadPid, "Edit");
		if (editWindow) { break; }
		Sleep(5);
	}

	if (!editWindow)
	{
		std::cout << "couldn't get eidt window\n";
		return 0;
	}

	if (!writeStringToNotepadBruteAndFindPointer())
	{
		return 0;
	}



	//exit(0);

	frameBuffer[0] = 't';
	frameBuffer[2] = 'e';
	frameBuffer[4] = 's';
	frameBuffer[6] = 't';
	
	UpdateText();

}

bool checkNotepadOpen()
{
	DWORD exitCode = 0;
	if (GetExitCodeProcess(notepadHandle, &exitCode))
	{
		if (exitCode == STILL_ACTIVE) { return 1; }
		return 0;
	}
	else
	{
		return 0;
	}
}

glm::ivec2 getNotepadBufferSize()
{
	return {sizeX, sizeY};
}

void writeInBuffer(glm::ivec2 pos, char c)
{
	if (pos.x < 0 || pos.y < 0 || pos.x >= sizeX || pos.y >= sizeY)
	{
		return;
	}

	frameBuffer[(pos.x + pos.y * (sizeX + 2)) * 2] = c;
	frameBuffer[(pos.x + pos.y * (sizeX + 4)) * 2+1] = 0;
}

void writeInBuffer(int x, int y, char c)
{
	writeInBuffer({x, y}, c);
}

void writeInBuffer(glm::ivec2 pos, char c, char c2)
{
	if (pos.x < 0 || pos.y < 0 || pos.x >= sizeX || pos.y >= sizeY)
	{
		return;
	}

	frameBuffer[(pos.x + pos.y * (sizeX + 2)) * 2] = c;
	frameBuffer[(pos.x + pos.y * (sizeX + 4)) * 2+1] = c2;
}


void writeInBuffer(int x, int y, char c, char c2)
{
	writeInBuffer({x, y}, c, c2);
}

namespace platform
{


	void setRelMousePosition(int x, int y)
	{
		POINT point = {x, y};
		ClientToScreen(editWindow, &point);
		SetCursorPos(point.x, point.y);
	}

	glm::ivec2 getRelMousePosition()
	{
		POINT point = {};
		GetCursorPos(&point);
		ScreenToClient(editWindow, &point);
		return {point.x, point.y};
	}

	glm::ivec2 getFrameBufferSize()//todo make constant
	{
		return {frameBufferSizeRect.right- frameBufferSizeRect.left, frameBufferSizeRect.bottom- frameBufferSizeRect.top};
	}

	void showMouse(bool show)
	{
		//intentionally removed
	}


	bool isFocused()
	{
		return GetActiveWindow() == notepadWindow;
	}


};