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

	std::vector<uint32_t>			pixels				= {};

	bool							running				= true;
	while(running) {
		rgb::windowUpdate(window);

		const uint32_t					windowArea			= uint32_t(window.Size.x * window.Size.y);

		pixels.resize(windowArea);

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
		if(drawSegment) {
			drawSegment = false;
			::rasterSegment(points[0], points[1], [=, &pixels, &window, &color] (const ::rgb::SCoord & cellCoord) {
				const rgb::SCircle					circle				= {cellCoord, 4};
				return rgb::rasterCircle(circle, [=, &pixels, &window, &color] (const ::rgb::SCoord & cellCoord) {
					pixels[cellCoord.x + window.Size.x * cellCoord.y] = color;	// Draw a pixel in the position of the mouse stored in our variable.
					return 1;
				});
			});
		}

		if(drawPencil) {
			const rgb::SCircle					circle				= {window.MousePosition, 4};
			rgb::rasterCircle(circle, [=, &pixels, &window, &color] (const ::rgb::SCoord & cellCoord) {
				pixels[cellCoord.x + window.Size.x * cellCoord.y] = color;	// Draw a pixel in the position of the mouse stored in our variable.
				return 1;
			});
		}

		const HDC								renderContext		= GetDC(window.Handle);	// Retrieve a handle to the drawing subsystem of the window..
		const HDC								compatibleContext	= CreateCompatibleDC(renderContext);	// Retrieve a handle to the drawing subsystem of the window..
		if(compatibleContext) {
			BITMAPINFO								bitmapInfo			= {sizeof(BITMAPINFO)};
			bitmapInfo.bmiHeader.biWidth		= window.Size.x;
			bitmapInfo.bmiHeader.biHeight		= window.Size.y;
			bitmapInfo.bmiHeader.biBitCount		= 32;
			bitmapInfo.bmiHeader.biPlanes		= 1;
			bitmapInfo.bmiHeader.biCompression	= BI_RGB;
			bitmapInfo.bmiHeader.biSizeImage	= windowArea * sizeof(uint32_t);

			uint32_t							* dibBytes			= 0;
			HBITMAP								dibSection			= CreateDIBSection(compatibleContext, &bitmapInfo, DIB_RGB_COLORS, (void**)&dibBytes, 0, 0);
			if(dibSection) {

				const uint32_t						rowSizeInBytes		= window.Size.x * sizeof(uint32_t);
				for(uint32_t y = 0; y < (uint32_t)window.Size.y; ++y) {
					const uint32_t						cellOffset			= y * window.Size.x;
					memcpy(&dibBytes[window.Size.y * window.Size.x - window.Size.x - cellOffset], &pixels[cellOffset], rowSizeInBytes);
				}
				HGDIOBJ								oldObject			= SelectObject(compatibleContext, dibSection);
				BitBlt(renderContext, 0, 0, window.Size.x, window.Size.y, compatibleContext, 0, 0, SRCCOPY);
				SelectObject(compatibleContext, oldObject);
				DeleteObject(dibSection);
			}
			DeleteDC(compatibleContext);
		}

		ReleaseDC(window.Handle, renderContext);	// Use this to tell the window. system we're not drawing anymore so it can draw its own window. elements such as the title bar, close/minimize/maximize buttons, etc. 
		// As drawing on the window. needs to be done in order, this GetDC() and ReleaseDC() allow window.s to know it's your turn to draw and it prevents the window. system to draw on the window. area until you tell you're done drawing.
	}

	return 0;
}