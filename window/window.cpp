#include <cstdio>
#include <windows.h>
#include <windowsx.h> // GET_X_LPARAM

LRESULT windowEventHandler(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
	switch(message) {
	case WM_MOUSEMOVE: {
		int mouseX = GET_X_LPARAM(lParam);
		int mouseY = GET_Y_LPARAM(lParam);
		printf("mouse X: %i, Y: %i\n"
			, mouseX 
			, mouseY
		);
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

	HWND							windowHandle		= CreateWindow(windowClass.lpszClassName, TEXT("My first window"), WS_OVERLAPPEDWINDOW, 0, 0, 640, 480, 0, 0, windowClass.hInstance, 0);
	ShowWindow(windowHandle, SW_SHOW);

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
	}

	return 0;
}