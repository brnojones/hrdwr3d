
#include "System.h"
//#include "System.h"

// windows entry point
int WINAPI wWinMain(HINSTANCE hInstance,			// instance handle
	HINSTANCE prevInstance,			// always NULL
	LPWSTR cmdLine, 				// cmdln params
	int cmdShow)					// how to show the window
{
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

	System system = System(hInstance, cmdShow, 800, 600);
	Game game = Game();

	game.Initialise();

	while (system.Update())
	{
		game.Update(0.13f);	//	Todo: Make a timer and pass in the delta time
		game.Render();
	}

	// shutdown

	return static_cast<int>(system.GetWindowMessage().wParam);

	return 0;
}
