#include "rgb_window.h"

LRESULT windowEventHandler(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
	std::queue<rgb::SWindowEvent>		& windowEventQueue		= *(std::queue<rgb::SWindowEvent>*)GetWindowLongPtr(windowHandle, GWLP_USERDATA);	// Get our rgb::SCoord instance defined in main()
	switch(message) {
	case WM_LBUTTONUP	: { rgb::SWindowEvent newEvent = {rgb::EVENT_MOUSE_BUTTON_UP	}; newEvent.Data.push_back(0); windowEventQueue.push(newEvent); break; }
	case WM_LBUTTONDOWN	: { rgb::SWindowEvent newEvent = {rgb::EVENT_MOUSE_BUTTON_DOWN}; newEvent.Data.push_back(0); windowEventQueue.push(newEvent); break; }
	case WM_RBUTTONUP	: { rgb::SWindowEvent newEvent = {rgb::EVENT_MOUSE_BUTTON_UP	}; newEvent.Data.push_back(1); windowEventQueue.push(newEvent); break; }
	case WM_RBUTTONDOWN	: { rgb::SWindowEvent newEvent = {rgb::EVENT_MOUSE_BUTTON_DOWN}; newEvent.Data.push_back(1); windowEventQueue.push(newEvent); break; }
	case WM_MBUTTONUP	: { rgb::SWindowEvent newEvent = {rgb::EVENT_MOUSE_BUTTON_UP	}; newEvent.Data.push_back(2); windowEventQueue.push(newEvent); break; }
	case WM_MBUTTONDOWN	: { rgb::SWindowEvent newEvent = {rgb::EVENT_MOUSE_BUTTON_DOWN}; newEvent.Data.push_back(2); windowEventQueue.push(newEvent); break; }
	case WM_MOUSEMOVE: {
		rgb::SCoord							mousePosition			= 
			{ GET_X_LPARAM(lParam)
			, GET_Y_LPARAM(lParam)
			};

		rgb::SWindowEvent					newEvent				= {rgb::EVENT_MOUSE_MOVE};
		newEvent.Data.resize(sizeof(mousePosition));
		*(rgb::SCoord*)&newEvent.Data[0]	= mousePosition;
		windowEventQueue.push(newEvent);
		break;
	}
	case WM_MOUSEWHEEL: {
		float							wheelDelta				= GET_WHEEL_DELTA_WPARAM(wParam) / float(WHEEL_DELTA);
		rgb::SWindowEvent					newEvent				= {rgb::EVENT_MOUSE_WHEEL};
		newEvent.Data.resize(sizeof(wheelDelta));
		*(float*)&newEvent.Data[0]	= wheelDelta;
		windowEventQueue.push(newEvent);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0); // post WM_QUIT to event queue
	}

	return DefWindowProc(windowHandle, message, wParam, lParam);
}

int rgb::windowCreate(SWindow & window) {
	window.Class.hInstance			= GetModuleHandle(NULL);
	window.Class.lpszClassName		= TEXT("MY_WINDOW_CLASS");
	window.Class.lpfnWndProc		= windowEventHandler;
	RegisterClass(&window.Class);

	window.Size						= {640, 480};	// Store the window. size here so we can use it in places other than the CreateWindow() call.
	window.EventQueue				= {};

	window.Handle					= CreateWindow(window.Class.lpszClassName, TEXT("My first window."), WS_OVERLAPPEDWINDOW, 0, 0, window.Size.x, window.Size.y, 0, 0, window.Class.hInstance, 0);
	window.MousePosition			= {};	// Store in this variable the mouse position in window. space.
	ShowWindow(window.Handle, SW_SHOW);
	SetWindowLongPtr(window.Handle, GWLP_USERDATA, (LONG_PTR)&window.EventQueue);	// Set the address of our mousePosition variable so the window.EventHandler() can retrieve it with GetWindowLongPtr() and assign new coordinates to it.
	return 0;
}

int rgb::windowUpdate(SWindow & window) {
	MSG								msg					= {};
	while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if(msg.message == WM_QUIT) {
			window.EventQueue.push({rgb::EVENT_QUIT});
		}
	}

	return 0;
}
