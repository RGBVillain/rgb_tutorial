#include "coord.h"

#include <cstdio> // printf()
#include <cstdint> // uint32_t
#include <vector> // 
#include <queue> // 
#include <windows.h> // CreateWindow(), SetWindowLongPtr(), RegisterClass(), GetDC(), ReleaseDC(), SetPixel()
#include <windowsx.h> // GET_X_LPARAM()

enum EVENT 
	{ EVENT_MOUSE_MOVE
	, EVENT_MOUSE_WHEEL
	, EVENT_MOUSE_BUTTON_UP
	, EVENT_MOUSE_BUTTON_DOWN
	};

struct SWindowEvent {
	EVENT				Type;
	std::vector<char>	Data;
};

LRESULT windowEventHandler(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
	std::queue<SWindowEvent>		& windowEventQueue		= *(std::queue<SWindowEvent>*)GetWindowLongPtr(windowHandle, GWLP_USERDATA);	// Get our SCoord instance defined in main()
	switch(message) {
	case WM_LBUTTONUP	: { SWindowEvent newEvent = {EVENT_MOUSE_BUTTON_UP	}; newEvent.Data.push_back(0); windowEventQueue.push(newEvent); break; }
	case WM_LBUTTONDOWN	: { SWindowEvent newEvent = {EVENT_MOUSE_BUTTON_DOWN}; newEvent.Data.push_back(0); windowEventQueue.push(newEvent); break; }
	case WM_RBUTTONUP	: { SWindowEvent newEvent = {EVENT_MOUSE_BUTTON_UP	}; newEvent.Data.push_back(1); windowEventQueue.push(newEvent); break; }
	case WM_RBUTTONDOWN	: { SWindowEvent newEvent = {EVENT_MOUSE_BUTTON_DOWN}; newEvent.Data.push_back(1); windowEventQueue.push(newEvent); break; }
	case WM_MBUTTONUP	: { SWindowEvent newEvent = {EVENT_MOUSE_BUTTON_UP	}; newEvent.Data.push_back(2); windowEventQueue.push(newEvent); break; }
	case WM_MBUTTONDOWN	: { SWindowEvent newEvent = {EVENT_MOUSE_BUTTON_DOWN}; newEvent.Data.push_back(2); windowEventQueue.push(newEvent); break; }
	case WM_MOUSEMOVE: {
		SCoord							mousePosition			= 
			{ GET_X_LPARAM(lParam)
			, GET_Y_LPARAM(lParam)
			};

		SWindowEvent					newEvent				= {EVENT_MOUSE_MOVE};
		newEvent.Data.resize(sizeof(mousePosition));
		*(SCoord*)&newEvent.Data[0]	= mousePosition;
		windowEventQueue.push(newEvent);
		break;
	}
	case WM_MOUSEWHEEL: {
		float							wheelDelta				= GET_WHEEL_DELTA_WPARAM(wParam) / float(WHEEL_DELTA);
		SWindowEvent					newEvent				= {EVENT_MOUSE_WHEEL};
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

int main() {
	printf("Hello world\n");

	WNDCLASS						windowClass			= {};
	windowClass.hInstance		= GetModuleHandle(NULL);
	windowClass.lpszClassName	= TEXT("MY_WINDOW_CLASS");
	windowClass.lpfnWndProc		= windowEventHandler;
	RegisterClass(&windowClass);

	SCoord							windowSize			= {640, 480};	// Store the window size here so we can use it in places other than the CreateWindow() call.
	std::queue<SWindowEvent>		windowEventQueue	= {};

	HWND							windowHandle		= CreateWindow(windowClass.lpszClassName, TEXT("My first window"), WS_OVERLAPPEDWINDOW, 0, 0, windowSize.x, windowSize.y, 0, 0, windowClass.hInstance, 0);
	ShowWindow(windowHandle, SW_SHOW);
	SetWindowLongPtr(windowHandle, GWLP_USERDATA, (LONG_PTR)&windowEventQueue);	// Set the address of our mousePosition variable so the windowEventHandler() can retrieve it with GetWindowLongPtr() and assign new coordinates to it.

	SCoord							mousePosition		= {};	// Store in this variable the mouse position in window space.
	uint32_t						colorIndex			= 0;	// 
	uint32_t						palette[]			=		// Define a list of colors to be drawn with the mouse
		{ 0xFF
		, 0xFF00
		, 0xFF0000
		, 0xFFFF
		, 0xFFFF00
		};


	bool							drawing				= false;

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

		while(windowEventQueue.size()) {
			const SWindowEvent				& nextEvent			= windowEventQueue.front();	// Get the next event to be processed.
			switch(nextEvent.Type) {	// Process depending on event type
			case EVENT_MOUSE_BUTTON_UP	: { drawing = false; break; }
			case EVENT_MOUSE_BUTTON_DOWN: { drawing = true; break; }
			case EVENT_MOUSE_WHEEL: {
				float							wheelDelta			= *(float*)&nextEvent.Data[0];	// Retrieve the wheel rotation delta from the event bytes.
				colorIndex					+= (wheelDelta >= 0) ? 1 : -1;
				colorIndex					%= std::size(palette);
				break;
			}
			case EVENT_MOUSE_MOVE: {
				mousePosition				= *(const SCoord*)&nextEvent.Data[0];	// Retrieve the mouse coordinates from the event data bytes.
				// 
				printf("mouse X: %i, Y: %i\n"	// we can now print the mouse position from outside of the event handler
					, mousePosition.x
					, mousePosition.y
				);
				break;
			}
			}
			windowEventQueue.pop();	// remove the processed message from the queue
		}
		if(drawing) {
			uint32_t						color				= palette[colorIndex];	// This is similar to HTML color values. Do your own research and try with other values to see the results.

			HDC								renderContext		= GetDC(windowHandle);	// Retrieve a handle to the drawing subsystem of the window.

			SCircle							circle				= {mousePosition, 4};
			rasterCircle(circle, [renderContext, color] (const SCoord & cellCoord) {
				SetPixel(renderContext, cellCoord.x, cellCoord.y, color);	// Draw a pixel in the position of the mouse stored in our variable.
				return 1;
			});

			ReleaseDC(windowHandle, renderContext);	// Use this to tell the window system we're not drawing anymore so it can draw its own window elements such as the title bar, close/minimize/maximize buttons, etc. 
			// As drawing on the window needs to be done in order, this GetDC() and ReleaseDC() allow windows to know it's your turn to draw and it prevents the window system to draw on the window area until you tell you're done drawing.
		}
	}

	return 0;
}