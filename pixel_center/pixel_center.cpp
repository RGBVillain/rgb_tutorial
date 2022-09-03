#include "rgb_window.h"

#include <cstdio> // printf()
#include <cstdint> // uint32_t


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

	std::vector<::rgb::SCoord>		points				= {};
	

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

		pixels.clear();
		pixels.resize(windowArea);

		while(window.EventQueue.size()) {
			const ::rgb::SWindowEvent				& nextEvent			= window.EventQueue.front();	// Get the next event to be processed.
			switch(nextEvent.Type) {	// Process depending on event type
			case ::rgb::EVENT_QUIT				: { running = false; break; }
			case ::rgb::EVENT_MOUSE_BUTTON_UP	: { 
				if(paintMode == PAINT_MODE_PENCIL) 
					drawPencil	= false; 
				else if(paintMode == PAINT_MODE_LINE) {
					if(points.size() % 2) {
						points.push_back(window.MousePosition);
						drawSegment				= false;
					}
				}
				break; 
			}
			case ::rgb::EVENT_MOUSE_BUTTON_DOWN	: { 
				if(paintMode == PAINT_MODE_PENCIL) 
					drawPencil = true; 
				else if(paintMode == PAINT_MODE_LINE) {
					if(0 == points.size() % 2) {
						points.push_back(window.MousePosition);
						drawSegment				= true;
					}
				}
				break; 
			}
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
		for(uint32_t iSegment = 0, totalSegments = (uint32_t)points.size() / 2; iSegment < totalSegments; ++iSegment) {
			::rgb::rasterSegment(points[iSegment * 2 + 0], points[iSegment * 2 + 1], [=, &pixels, &window, &color] (const ::rgb::SCoord & cellCoord) {
				const ::rgb::SCircle					circle				= {cellCoord, 4};
				return ::rgb::rasterCircle(circle, [=, &pixels, &window, &color] (const ::rgb::SCoord & cellCoord) {
					pixels[cellCoord.x + window.Size.x * cellCoord.y] = color;	// Draw a pixel in the position of the mouse stored in our variable.
					return 1;
				});
			});
		}

		if(drawSegment) {
			::rgb::rasterSegment(points[points.size() - 1], window.MousePosition, [=, &pixels, &window, &color] (const ::rgb::SCoord & cellCoord) {
				const ::rgb::SCircle					circle				= {cellCoord, 4};
				return ::rgb::rasterCircle(circle, [=, &pixels, &window, &color] (const ::rgb::SCoord & cellCoord) {
					pixels[cellCoord.x + window.Size.x * cellCoord.y] = color;	// Draw a pixel in the position of the mouse stored in our variable.
					return 1;
				});
			});
		}

		if(drawPencil) {
			const ::rgb::SCircle					circle				= {window.MousePosition, 4};
			::rgb::rasterCircle(circle, [=, &pixels, &window, &color] (const ::rgb::SCoord & cellCoord) {
				pixels[cellCoord.x + window.Size.x * cellCoord.y] = color;	// Draw a pixel in the position of the mouse stored in our variable.
				return 1;
			});
		}
		::rgb::windowPresent(window, pixels.data());
	}

	return 0;
}