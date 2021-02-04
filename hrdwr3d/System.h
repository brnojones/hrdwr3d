
#pragma once

#include "Window.h"
#include "DirX.h"
#include "Game.h"

class System
{
public:
	System(HINSTANCE hInstance, int cmdShow, int width, int height);
	~System();

	bool Update();

	MSG GetWindowMessage();

private:
	Window _window;
	DirX _dx;

	bool _quit = false;

	void Quit();
	bool ShouldQuit();
};