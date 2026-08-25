#include "TileUtilities.h"

std::unique_ptr<wxBitmap> TileUtilities::createWXBitmap(Tile * tile, ColourTable * colourTable) {
	if(tile == nullptr || tile->isEmpty()) {
		return nullptr;
	}

	const uint16_t width = tile->getWidth();
	const uint16_t height = tile->getHeight();
	const std::vector<uint8_t> & pixels = tile->getPixels();
	wxImage tileImage(static_cast<int>(width), static_cast<int>(height), false);
	uint8_t * tileImageData = tileImage.GetData();

	for(size_t x = 0; x < width; ++x) {
		for(size_t y = 0; y < height; ++y) {
			const uint8_t pixel = pixels[(x * height) + y];
			const Colour & colour = colourTable->getColour(pixel);
			const size_t destinationPixelIndex = ((y * width) + x) * 3;

			tileImageData[destinationPixelIndex + 0] = colour.r;
			tileImageData[destinationPixelIndex + 1] = colour.g;
			tileImageData[destinationPixelIndex + 2] = colour.b;
		}
	}

	return std::make_unique<wxBitmap>(std::move(tileImage));
}
