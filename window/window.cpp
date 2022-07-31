#include <cstdio>		// Contains declaration of printf()
#include <windows.h>	// Contains declaration of windows API functions such as RegisterClass, CreateWindow, ShowWindow, PeekMessage, TranslateMessage, DispatchMessage, and types WNDCLASS, LRESULT, UINT, WPARAM, LPARAM
#include <windowsx.h>	// Contains definition of the helpers GET_X_LPARAM() and GET_Y_LPARAM

// The window event handler will be called by DispatchMessage() every time PeekMessage() returns true after assigning the message data to our MSG variable
LRESULT windowEventHandler(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
	case WM_MOUSEMOVE: { // WM_MOUSEMOVE is posted by Windows when it detects the mouse cursor moved over our window.
		// Get the position of the mouse in client space (the drawing area of the window)
		int mouseX = GET_X_LPARAM(lParam);	
		int mouseY = GET_Y_LPARAM(lParam);	
		printf("mouse X: %i, Y: %i\n"	// Print the position of the mouse
			, mouseX 
			, mouseY
		);
		break;
	}
	case WM_DESTROY: // WM_DESTROY is posted by the window subsystem when the window is closed by pressing the X button, alt+F4 or some other way of closing the window
		PostQuitMessage(0); // post WM_QUIT to event queue
	}

	return DefWindowProc(windowHandle, message, wParam, lParam);	// call the default window procedure for default handling of events we didn't process.
}

// main() is the entry point of our program, equivalent to the entry point as defined in Python, Java, etc.
int main() {
	printf("Hello world\n");

	WNDCLASS						windowClass			= {};	// This variable holds some values we need to register the window class
	windowClass.hInstance		= GetModuleHandle(NULL);
	windowClass.lpszClassName	= TEXT("MY_WINDOW_CLASS");
	windowClass.lpfnWndProc		= windowEventHandler;
	RegisterClass(&windowClass); // Register the class so we can create instances of it calling CreateWindow()

	// 
	HWND							windowHandle		= CreateWindow(windowClass.lpszClassName, TEXT("My first window"), WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, 0, 0, windowClass.hInstance, 0);
	ShowWindow(windowHandle, SW_SHOW);	// The window is created hidden. Call this functiom to make it visible.

	bool							running				= true;
	while(running) { // Execute the main loop until we tell it to stop
		MSG								msg					= {};	// Declare this variable to be filled with the next event data found in the window event queue
		while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {	// See if there is a message in the window event queue
			TranslateMessage(&msg);	// This function allows windows to generate additional messages if required by the event
			DispatchMessage(&msg);	// This function calls the window event handlers assigned to the window
			if(msg.message == WM_QUIT) { // PostQuitMessage(0) posts a WM_QUIT message to the window queue.
				running = false;	// end our main loop
			}
		}
		// 
	}

	return EXIT_SUCCESS;	// Exit the application
}