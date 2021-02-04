
#pragma once

#include<Windows.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

class Window
{
public:
	Window();
	~Window();

	bool Initialise(HINSTANCE hInstance, int cmdShow, int width, int height);
	bool Update();

	MSG GetMessage();
	HWND* GetWindowHandle();

private:
	MSG _msg;
	HWND _hwnd;

	void MessagePump();
};