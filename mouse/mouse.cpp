#include <cstdio>
#include <cstdint>
#include <windows.h>
#include <windowsx.h> // GET_X_LPARAM

// We're going to use this structure a lot for handling user input, drawing on the screen, calculate physics and animations, make an UI system, etc.
struct SCoord {
	int x;
	int y;
};

LRESULT windowEventHandler(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
	case WM_MOUSEMOVE: {
		SCoord							* mousePosition			= (SCoord*)GetWindowLongPtr(windowHandle, GWLP_USERDATA);	// Get our SCoord instance defined in main()
		*mousePosition				= // Assign the mouse coordinate values sent by the operating system
			{ GET_X_LPARAM(lParam)
			, GET_Y_LPARAM(lParam)
			};
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0); // post WM_QUIT to event queue
	}

	return DefWindowProc(windowHandle, message, wParam, lParam);
}

int main() {
	printf("Hello world\n");

	WNDCLASS						windowClass			= {};
	windowClass.hInstance		= GetModuleHandle(NULL);
	windowClass.lpszClassName	= TEXT("MY_WINDOW_CLASS");
	windowClass.lpfnWndProc		= windowEventHandler;
	RegisterClass(&windowClass);

	SCoord							windowSize			= {640, 480};	// Store the window size here so we can use it in places other than the CreateWindow() call.
	SCoord							mousePosition		= {};			// Store in this variable the mouse position in window space.

	HWND							windowHandle		= CreateWindow(windowClass.lpszClassName, TEXT("My first window"), WS_OVERLAPPEDWINDOW, 0, 0, windowSize.x, windowSize.y, 0, 0, windowClass.hInstance, 0);
	ShowWindow(windowHandle, SW_SHOW);
	SetWindowLongPtr(windowHandle, GWLP_USERDATA, (LONG_PTR)&mousePosition);	// Set the address of our mousePosition variable so the windowEventHandler() can retrieve it with GetWindowLongPtr() and assign new coordinates to it.


	bool							running				= true;
	while(running) {
		MSG								msg					= {};
		while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if(msg.message == WM_QUIT) {
				running = false;
			}
		}
		// 
		printf("mouse X: %i, Y: %i\n"	// we can now print the mouse position from outside of the event handler
			, mousePosition.x
			, mousePosition.y
		);
		uint32_t						color				= 0x000000FF;	// This is similar to HTML color values. Do your own research and try with other values to see the results.

		HDC								renderContext		= GetDC(windowHandle);	// Retrieve a handle to the drawing subsystem of the window.
		SetPixel(renderContext, mousePosition.x, mousePosition.y, color);	// Draw a pixel in the position of the mouse stored in our variable.
		ReleaseDC(windowHandle, renderContext);	// Use this to tell the window system we're not drawing anymore so it can draw its own window elements such as the title bar, close/minimize/maximize buttons, etc. 
		// As drawing on the window needs to be done in order, this GetDC() and ReleaseDC() allow windows to know it's your turn to draw and it prevents the window system to draw on the window area until you tell you're done drawing.
	}

	return 0;
}