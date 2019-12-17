#include "platformIndependentCalls.h"

#ifdef MGL_WIN

#include <Windows.h>
#include "game.h"
#include <GL/glew.h>
#include <ctime>

LRESULT CALLBACK windProc(HWND, UINT, WPARAM, LPARAM);
static bool quit = 0;

void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);

HWND wind;

static int lbutton = 0;
static int rbutton = 0;
static int lbuttonPressed = 0;
static int rbuttonPressed = 0;


int APIENTRY WinMain(HINSTANCE h, HINSTANCE, LPSTR cmd, int show)
{
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
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		heigth,
		NULL,
		NULL,
		h,
		0
	);

	HDC hdc;
	HGLRC hrc;

	EnableOpenGL(wind, &hdc, &hrc);
	if (glewInit() != GLEW_OK)
	{
		MessageBoxA(0, "glewInit", "Error from glew", MB_ICONERROR);
		return 1;
	}

	if (!initGame())
	{
		return 0;
	}

	ShowCursor(0);

	int time1 = clock();
	int time2 = clock();

	while(!quit)
	{
		MSG msg;
		if(PeekMessage(&msg, wind, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}else
		{
			time2 = clock();
			int deltaTime = time2 - time1;
			time1 = clock();
			
			if (!gameLogic((float)deltaTime / CLOCKS_PER_SEC))
			{
				quit = true;
			}

			SwapBuffers(hdc);
		
			lbuttonPressed = false;
			rbuttonPressed = false;
		}
	
	}

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

	case WM_CLOSE:
		quit = true;

	default:
		l = DefWindowProc(wind, m, wp, lp);
		break;
	}

	return l;
}


void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
	//todo look into this more (all of this function)
	PIXELFORMATDESCRIPTOR pfd = {};

	int iFormat;

	/* get the device context (DC) */
	*hDC = GetDC(hwnd);


	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32; //todo look into this (24)
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	iFormat = ChoosePixelFormat(*hDC, &pfd);

	SetPixelFormat(*hDC, iFormat, &pfd);

	/* create and enable the render context (RC) */
	*hRC = wglCreateContext(*hDC);

	wglMakeCurrent(*hDC, *hRC);
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

void showMouse(bool show)
{
}

#endif // MGL_WIN