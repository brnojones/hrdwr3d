
#include "System.h"


System::System(HINSTANCE hInstance, int cmdShow, int width, int height)
	: _window(), _dx()
{
	if (!_window.Initialise(hInstance, cmdShow, width, height))
	{
		//	Assert
		Quit();
	}

	if (!_dx.Initialise(_window.GetWindowHandle(), width, height))
	{
		//	Assert
		Quit();
	}
}

System::~System()
{

}

bool System::Update()
{
	if (ShouldQuit())
	{
		return false;
	}

	if (!_window.Update())
	{
		return false;
	}

	//	Update and draw
	_dx.Update(0.13f);

	_dx.Render();

	return true;
}

void System::Quit()
{
	_quit = true;
}

bool System::ShouldQuit()
{
	if (GetWindowMessage().message == WM_QUIT)
	{
		return true;
	}
	if (_quit)
	{
		return true;
	}
	return false;
}

MSG System::GetWindowMessage()
{
	return _window.GetMessage();
}

