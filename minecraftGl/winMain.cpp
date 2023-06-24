#include "platformIndependentCalls.h"

#ifdef MGL_WIN

#include <Windows.h>
#include "game.h"
#include <GL/glew.h>
#include "GL/wglew.h"

#include <ctime>
#include <stdio.h>
//#include <io.h>
//#include <fcntl.h>

#include "tools.h"
#include "input.h"
#include "opengl2Dlib.h"

LRESULT CALLBACK windProc(HWND, UINT, WPARAM, LPARAM);
static bool quit = 0;

void EnableOpenGL(HDC, HGLRC*);
int initOpengl(HDC, HGLRC*);

HWND wind;

static int lbutton = 0;
static int rbutton = 0;
static int lbuttonPressed = 0;
static int rbuttonPressed = 0;
static int bMouseMoved = 0;

static bool isFocus = 0;

#define GPU_ENGINE 1
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = GPU_ENGINE;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = GPU_ENGINE;
}


GameData gameData = {};

int MAIN
{

	input::loadXinput();

	HINSTANCE h = GetModuleHandle(0);

	WNDCLASS wc = {};
	wc.hInstance = h;
	wc.lpfnWndProc = windProc;
	wc.lpszClassName = "wclass";
	wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClass(&wc);

	float width = 960;
	float heigth = 680;

	wind = CreateWindow(
		wc.lpszClassName,
		"Minicraft",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		550,
		100,
		width,
		heigth,
		NULL,
		NULL,
		h,
		0
	);

	HDC hdc = GetDC(wind);
	HGLRC hrc = {};

	initOpengl(hdc, &hrc);

	gl2d::init();
	gl2d::setErrorFuncCallback([](const char* c) {elog(c); });

	if (!initGame())
	{
		return 0;
	}

	showMouse(false);

	int time1 = clock();
	int time2 = clock();

	float timeCounter = 0;
	int frameCounter = 0;
	int lastfps = 0;

	gameData.vendor = glGetString(GL_VENDOR);
	gameData.renderer = glGetString(GL_RENDERER);

	while(!quit)
	{
		MSG msg;
		if(PeekMessage(&msg, wind, 0, 0, PM_REMOVE)>0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}else
		{
			time2 = clock();
			int deltaTime = time2 - time1;
			time1 = clock();
			
			float fDeltaTime = (float)deltaTime / CLOCKS_PER_SEC;

			timeCounter += fDeltaTime;
			frameCounter ++;

			if(timeCounter>1)
			{
				timeCounter -= 1;
				lastfps = frameCounter;
				frameCounter = 0;
			}


			fDeltaTime = min(fDeltaTime, 1.f / 20.f);

			input::updateInput();

			gameData.fps = lastfps;

			if (!gameLogic(fDeltaTime, gameData))
			{
				quit = true;
			}

			SwapBuffers(hdc);
		
			lbuttonPressed = false;
			rbuttonPressed = false;
			bMouseMoved = false;
		}
	
	}

	closeGame();

	return 0;
}

LRESULT CALLBACK windProc(HWND wind, UINT m, WPARAM wp, LPARAM lp)
{
	LRESULT l = 0;

	switch (m)
	{
	case WM_LBUTTONDOWN:
		lbutton = true;
		lbuttonPressed = true;
			break;
	case WM_RBUTTONDOWN:
		rbutton = true;
		rbuttonPressed = true;
		break;
	case WM_RBUTTONUP:
		rbutton = false;
		break;
	case WM_LBUTTONUP:
		lbutton = false;
		break;
	case WM_MOUSEMOVE:
		bMouseMoved = 1;
		break;

	case WM_CLOSE:
		quit = true;
		break;

	case WM_ACTIVATE:
		if(wp == WA_ACTIVE)
		{
			isFocus = true;
		}else if(wp == WA_INACTIVE)
		{
			isFocus = false;
		}
		break;

	default:
		l = DefWindowProc(wind, m, wp, lp);
		break;
	}

	return l;
}

void EnableOpenGL(HDC hDC, HGLRC* hRC)
{

	PIXELFORMATDESCRIPTOR pfd = {};

	int iFormat = 0;

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 16;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(hDC, &pfd);

	SetPixelFormat(hDC, iFormat, &pfd);

	/* create and enable the render context (RC) */
	*hRC = wglCreateContext(hDC);

	wglMakeCurrent(hDC, *hRC);

	if (glewInit() != GLEW_OK)
	{
		MessageBoxA(0, "glewInit", "Error from glew", MB_ICONERROR);
		ExitProcess(1);
	}

	int nPixelFormat2 = 0;

	BOOL bValidPixFormat = 0;
	UINT nNumFormats = 0;
	float pfAttribFList[] = { 0 };
	int piAttribIList[] = {
		//WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		//WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		//WGL_COLOR_BITS_ARB, 32,
		//WGL_RED_BITS_ARB, 8,
		//WGL_GREEN_BITS_ARB, 8,
		//WGL_BLUE_BITS_ARB, 8,
		//WGL_ALPHA_BITS_ARB, 8,
		//WGL_DEPTH_BITS_ARB, 16,
		//WGL_STENCIL_BITS_ARB, 0,
		//WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		//WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		//WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		//WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
		//WGL_SAMPLES_ARB, 4,
		//0, 0 
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_SAMPLE_BUFFERS_ARB, 1, // Number of buffers (must be 1 at time of writing)
		WGL_SAMPLES_ARB, 8,        // Number of samples
		0
	};

	const char *c =(const char*)glGetString(GL_EXTENSIONS);

	bValidPixFormat = wglChoosePixelFormatARB(hDC, piAttribIList, pfAttribFList, 1, &nPixelFormat2, &nNumFormats);


	if (!bValidPixFormat)
	{
		MessageBox(NULL, "Invalid Pixel Format", "Error! (SetupWGLPixelFormat)", MB_OK);
	}

	if(!SetPixelFormat(hDC, nPixelFormat2, &pfd))
	{
		elog("setPFormat", GetLastError());
	}

	wglMakeCurrent(hDC, NULL);
	wglDeleteContext(*hRC);

	*hRC = wglCreateContext(hDC);
	
	wglMakeCurrent(hDC, *hRC);
}


LRESULT CALLBACK dumyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int initOpengl(HDC hDC1, HGLRC* hRC1)
{
	HWND hwnd;
	HINSTANCE hInstance = GetModuleHandle(NULL);

	//---- fake Window
	WNDCLASSEX wcex = {};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = dumyWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	//wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	//wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "dumy";
	wcex.hIconSm = NULL;

	if (!RegisterClassEx(&wcex))
	{
		elog("RegisterClassEx");
		return 0;
	}

	hwnd = CreateWindow(
		"dumy",
		"",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		500, 500,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	HDC hdc = GetDC(hwnd);

	PIXELFORMATDESCRIPTOR pfd = {};

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(hdc, &pfd);

	SetPixelFormat(hdc, nPixelFormat, &pfd);

	HGLRC hrc = wglCreateContext(hdc);

	wglMakeCurrent(hdc, hrc);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		MessageBoxA(0, "glewInit", "Error from glew", MB_ICONERROR);
		ExitProcess(1);
	}

	//---------------For the real window
	if (wglewIsSupported("WGL_ARB_create_context") == 1 &&
		wglewIsSupported("WGL_ARB_pixel_format") == 1)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hrc);
		ReleaseDC(hwnd, hdc);
		DestroyWindow(hwnd);
		UnregisterClass("dumy", hInstance);

		const int iPixelFormatAttribList[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			//WGL_SAMPLE_BUFFERS_ARB, 1, // Number of buffers (must be 1 at time of writing)
			//WGL_SAMPLES_ARB, 2,        // Number of samples
			0
		};

		int attributes[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 3
			, WGL_CONTEXT_MINOR_VERSION_ARB, 2
			, WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
			, 0
		};

		int nPixelFormat = 0;
		UINT iNumFormats = 0;

		wglChoosePixelFormatARB(hDC1, iPixelFormatAttribList, NULL, 1, &nPixelFormat, (UINT*)&iNumFormats);

		if(!SetPixelFormat(hDC1, nPixelFormat, &pfd))
		{
			wlog("SetPixelFormat", GetLastError());
		}

		*hRC1 = wglCreateContext(hDC1);
		//*hRC1 = wglCreateContextAttribsARB(hDC1, 0, attributes);

		wglMakeCurrent(hDC1, *hRC1);

		if (wglGetProcAddress("wglSwapIntervalEXT") != nullptr)
		{
			wglSwapIntervalEXT(true);
		}

	}
	else
	{
		wlog("WGL_ARB_create_context and WGL_ARB_pixel_format");
		return 0;
	}
	return true;
}

//platform independent functions implementation
///////////////////////////////////////////////
void errorMessage(const char *c)
{
	MessageBoxA(0, c, "error", MB_ICONERROR);
}

///sets the mouse pos relative to the window's drawing area
void setRelMousePosition(int x, int y)
{
	POINT p = {x, y};
	WINDOWPLACEMENT wp;

	GetWindowPlacement(wind, &wp);
	p.x += wp.rcNormalPosition.left;
	p.y += wp.rcNormalPosition.top;

	SetCursorPos(p.x, p.y);
}

///gets the mouse pos relative to the window's drawing area
glm::ivec2 getRelMousePosition()
{
	//todo refactor

	POINT p = {};
	GetCursorPos(&p);

	WINDOWPLACEMENT wp;

	GetWindowPlacement(wind, &wp);
	
	p.x -= wp.rcNormalPosition.left;
	p.y -= wp.rcNormalPosition.top;

	return {p.x, p.y};
}

//gets the drawing region sizes
glm::ivec2 getWindowSize()
{
	RECT r;
	GetClientRect(wind, &r);
	glm::ivec2 d;
	d.x = r.right - r.left;
	d.y = r.bottom - r.top;
	return d;
}

int isKeyPressed(int key)
{
	return GetAsyncKeyState(key);
}

int isKeyPressedOn(int key)
{
	return GetAsyncKeyState(key) & 0x8000;
}

int isLMouseButtonPressed()
{
	return lbuttonPressed;
}

int isRMouseButtonPressed()
{
	return rbuttonPressed;
}

int isLMouseHeld()
{
	return lbutton;
}

int isRMouseHeld()
{
	return rbutton;
}

bool mouseMoved()
{
	return bMouseMoved;
}

static bool lastShow = 1;

int mouseVizibility = 1;

void showMouse(bool show)
{
	if (lastShow != show)
	{
		ShowCursor(show);

		if (show)
		{
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			SendMessage(wind, WM_SETCURSOR, (WPARAM)wind, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
			//SetCursor(LoadCursor(GetModuleHandle(0), IDC_HAND));
		}
	}

	lastShow = show;
}
bool isFocused()
{
	return GetActiveWindow() == wind;
}

#endif // MGL_WIN
