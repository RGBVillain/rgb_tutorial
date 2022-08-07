//#pragma once
#include <functional>

namespace rgb 
{
	// We're going to use this structure a lot for handling user input, drawing on the screen, calculate physics and animations, make an UI system, etc.
	struct SCoord {
		int					x;
		int					y;

		// This allows the operation SCoord(a) - SCoord(b)
		SCoord				operator -		(const SCoord &other)	const	{ return {x - other.x, y -  other.y}; }

		int					sqLength		()						const	{ return x * x + y * y; } 
		double				length			()						const	{ // Returns the length of the vector represented by the members x and y 
			int						squareLength	= sqLength(); 
			return squareLength ? sqrt(squareLength) : 0; // don't calculate square root for zero-length vectors
		}
	};

	struct SCircle {
		SCoord				Center;
		float				Radius;
	};

	struct SRectangle {
		SCoord				Position;
		SCoord				Size;
	};

	int rasterRectangle (const SRectangle & rectangle, std::function<int(const SCoord &)> funcSetPixel);
	int rasterCircle	(const SCircle & circle, std::function<int(const SCoord &)> funcSetPixel);
}