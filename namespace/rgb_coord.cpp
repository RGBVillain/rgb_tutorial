#include "rgb_coord.h"

int rgb::rasterRectangle (const SRectangle & rectangle, std::function<int(const SCoord &)> funcSetPixel) {
	int					cellsDrawn		= 0;
	for(int y = rectangle.Position.y, yStop = rectangle.Position.y + rectangle.Size.y; y < yStop; ++y)
	for(int x = rectangle.Position.x, xStop = rectangle.Position.x + rectangle.Size.x; x < xStop; ++x)
		cellsDrawn		+= funcSetPixel(SCoord{x, y});

	return cellsDrawn;
}

int rgb::rasterCircle	(const SCircle & circle, std::function<int(const SCoord &)> funcSetPixel) {
	const SRectangle	enclosingRectangle	= 
		{ {int(circle.Center.x - circle.Radius), int(circle.Center.y - circle.Radius)}
		, {int(circle.Center.x + circle.Radius), int(circle.Center.y - circle.Radius)}
		};

	return rasterRectangle(enclosingRectangle, [circle, funcSetPixel] (const SCoord & cellCoord) {
		if((cellCoord - circle.Center).length() <= circle.Radius)
			return funcSetPixel(cellCoord);
		return 0;
	});
}