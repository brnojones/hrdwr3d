
#include "Window.h"
// #include<iostream>
// #include<sstream>


Window::Window()
{
	//if (!Initialise(hInstance, cmdShow))
	{
		//	Assert
	}
}

Window::~Window()
{

}

bool Window::Update()
{
	MessagePump();
	return true;
}

MSG Window::GetMessage()
{
	return _msg;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;
	HDC hDC;

	// 	char c;
	// 	std::string s;
	// 	std::stringstream ss;

	switch (message)
	{
	case WM_PAINT:
		hDC = BeginPaint(hwnd, &paintStruct);
		EndPaint(hwnd, &paintStruct);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		// 			c = static_cast<unsigned char>(wParam);
		// 			ss<<c;
		// 			ss>>s;
		// 			SetWindowText(hwnd, s.c_str());
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	return 0;
}

bool Window::Initialise(HINSTANCE hInstance, int cmdShow, int width, int height)
{
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = "wndClass";

	if (!RegisterClassEx(&wndClass))
	{
		return false;
	}

	RECT rc = { 0, 0, width, height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	_hwnd = CreateWindowA("wndClass",				// classname
		"Window Name",			// window name
		WS_OVERLAPPEDWINDOW, 		// window styles
		CW_USEDEFAULT, 			// x
		CW_USEDEFAULT, 			// y
		rc.right - rc.left,		// w				
		rc.bottom - rc.top, 		// h
		NULL, 					// parent
		NULL, 					// menu
		hInstance, 				// instance handle
		NULL);					// custom param
	if (!_hwnd)
	{
		return false;
	}

	ShowWindow(_hwnd, cmdShow);

	// Other init?

	return true;
}

void Window::MessagePump()
{
	if (PeekMessage(&_msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&_msg);
		DispatchMessage(&_msg);
	}
}

HWND* Window::GetWindowHandle()
{
	return &_hwnd;
}
