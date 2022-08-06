//#pragma once
#include <functional>

// We're going to use this structure a lot for handling user input, drawing on the screen, calculate physics and animations, make an UI system, etc.
struct SCoord {
	int					x;
	int					y;

	// This allows the operation SCoord(a) - SCoord(b)
	SCoord				operator -		(const SCoord &other)	const	{ return {x - other.x, y -  other.y}; }

	int					sqLength		()						const	{ return x * x + y * y; } 
	float				length			()						const	{ // Returns the length of the vector represented by the members x and y 
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

int rasterRectangle (const SRectangle & rectangle, std::function<int(const SCoord &)> funcSetPixel) {
	int					cellsDrawn		= 0;
	for(int y = rectangle.Position.y, yStop = rectangle.Position.y + rectangle.Size.y; y < yStop; ++y)
	for(int x = rectangle.Position.x, xStop = rectangle.Position.x + rectangle.Size.x; x < xStop; ++x)
		cellsDrawn		+= funcSetPixel(SCoord{x, y});

	return cellsDrawn;
}

int rasterCircle	(const SCircle & circle, std::function<int(const SCoord &)> funcSetPixel) {
	const SRectangle	enclosingRectangle	= 
		{ {circle.Center.x - circle.Radius, circle.Center.y - circle.Radius}
		, {circle.Center.x + circle.Radius, circle.Center.y - circle.Radius}
		};

	return rasterRectangle(enclosingRectangle, [circle, funcSetPixel] (const SCoord & cellCoord) {
		if((cellCoord - circle.Center).length() <= circle.Radius)
			return funcSetPixel(cellCoord);
		});
}
