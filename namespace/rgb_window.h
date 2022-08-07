#include "rgb_coord.h"

#include <vector> // 
#include <queue> // 
#include <windows.h> // CreateWindow(), SetWindowLongPtr(), RegisterClass(), GetDC(), ReleaseDC(), SetPixel()
#include <windowsx.h> // GET_X_LPARAM()

namespace rgb
{
	enum EVENT 
		{ EVENT_QUIT
		, EVENT_MOUSE_MOVE
		, EVENT_MOUSE_WHEEL
		, EVENT_MOUSE_BUTTON_UP
		, EVENT_MOUSE_BUTTON_DOWN
		};

	struct SWindowEvent {
		EVENT				Type;
		std::vector<char>	Data;
	};

	struct SWindow {
		HWND								Handle			= {};
		WNDCLASS							Class			= {};
		rgb::SCoord							Size			= {640, 480};	// Store the window size here so we can use it in places other than the CreateWindow() call.
		std::queue<rgb::SWindowEvent>		EventQueue		= {};
		rgb::SCoord							MousePosition	= {};	// Store in this variable the mouse position in window space.
	};

	int windowCreate(SWindow & window);
	int windowUpdate(SWindow & window);
}
