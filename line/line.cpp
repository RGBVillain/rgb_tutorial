#include "rgb_window.h"

#include <cstdio> // printf()
#include <cstdint> // uint32_t

int				rasterSegment			(::rgb::SCoord A, ::rgb::SCoord B, const std::function<int(const ::rgb::SCoord &)> & funcSetPixel) {
	int				countPixels				= 0;
	::rgb::SCoord	difference				= {(B.x - A.x), (B.y - A.y)};
	bool			useX					= abs(difference.x) > abs(difference.y);

	if(useX) {
		if(A.x > B.x) {
			std::swap(A, B);
			difference		= B - A;
		}
		float			slope					= difference.y / float(difference.x);
		float			y						= float(A.y);
		for(int x = A.x; x < B.x; ++x) {
			countPixels	+= funcSetPixel({x, int(y)});
			y			+= slope;
		}
	}
	else {
		if(A.y > B.y) {
			std::swap(A, B);
			difference		= B - A;
		}
		float			slope					= difference.x / float(difference.y);
		float			x						= float(A.x);
		for(int y = A.y; y < B.y; ++y) {
			countPixels	+= funcSetPixel({int(x), y});
			x			+= slope;
		}
	}

	return countPixels;
}

int				main					() {
	printf("Hello world\n");

	rgb::SWindow					window;
	rgb::windowCreate(window);

	uint32_t						colorIndex			= 0;	// 
	uint32_t						palette[]			=		// Define a list of colors to be drawn with the mouse
		{ 0xFF
		, 0xFF00
		, 0xFF0000
		, 0xFFFF
		, 0xFFFF00
		};

	bool							drawPencil			= false;
	bool							drawSegment			= false;
	::rgb::SCoord					points[2]			= {};
	uint32_t						currentPoint		= 0;

	enum PAINT_MODE	
		{ PAINT_MODE_PENCIL
		, PAINT_MODE_LINE
		};

	PAINT_MODE						paintMode			= PAINT_MODE_LINE;

	bool							running				= true;
	while(running) {
		rgb::windowUpdate(window);

		while(window.EventQueue.size()) {
			const ::rgb::SWindowEvent				& nextEvent			= window.EventQueue.front();	// Get the next event to be processed.
			switch(nextEvent.Type) {	// Process depending on event type
			case ::rgb::EVENT_QUIT				: { running = false; break; }
			case ::rgb::EVENT_MOUSE_BUTTON_UP	: { 
				if(paintMode == PAINT_MODE_PENCIL) 
					drawPencil	= false; 
				else if(paintMode == PAINT_MODE_LINE) {
					points[currentPoint++]	= window.MousePosition;
					currentPoint			%= 2;
					if(0 == currentPoint)
						drawSegment				= true;
				}
				break; 
			}
			case ::rgb::EVENT_MOUSE_BUTTON_DOWN	: { if(paintMode == PAINT_MODE_PENCIL) drawPencil = true; break; }
			case ::rgb::EVENT_MOUSE_WHEEL		: {
				float							wheelDelta			= *(float*)&nextEvent.Data[0];	// Retrieve the wheel rotation delta from the event bytes.
				colorIndex					+= (wheelDelta >= 0) ? 1 : -1;
				colorIndex					%= std::size(palette);
				break;
			}
			case ::rgb::EVENT_MOUSE_MOVE: {
				window.MousePosition				= *(const ::rgb::SCoord*)&nextEvent.Data[0];	// Retrieve the mouse coordinates from the event data bytes.
				// 
				printf("mouse X: %i, Y: %i\n"	// we can now print the mouse position from outside of the event handler
					, window.MousePosition.x
					, window.MousePosition.y
				);
				break;
			}
			}
			window.EventQueue.pop();	// remove the processed message from the queue
		}

		const uint32_t						color				= palette[colorIndex];	// This is similar to HTML color values. Do your own research and try with other values to see the results.
		const HDC							renderContext		= GetDC(window.Handle);	// Retrieve a handle to the drawing subsystem of the window..

		if(drawSegment) {
			drawSegment = false;
			::rasterSegment(points[0], points[1], [renderContext, color] (const ::rgb::SCoord & cellCoord) {
				SetPixel(renderContext, cellCoord.x, cellCoord.y, color);	// Draw a pixel in the position of the mouse stored in our variable.
				return 1;
			});
		}

		if(drawPencil) {
			const rgb::SCircle					circle				= {window.MousePosition, 4};
			rgb::rasterCircle(circle, [renderContext, color] (const ::rgb::SCoord & cellCoord) {
				SetPixel(renderContext, cellCoord.x, cellCoord.y, color);	// Draw a pixel in the position of the mouse stored in our variable.
				return 1;
			});
		}


		ReleaseDC(window.Handle, renderContext);	// Use this to tell the window. system we're not drawing anymore so it can draw its own window. elements such as the title bar, close/minimize/maximize buttons, etc. 
		// As drawing on the window. needs to be done in order, this GetDC() and ReleaseDC() allow window.s to know it's your turn to draw and it prevents the window. system to draw on the window. area until you tell you're done drawing.
	}

	return 0;
}