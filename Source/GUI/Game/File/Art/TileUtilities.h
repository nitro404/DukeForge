#include <memory>

#include "Game/File/Art/Tile.h"
#include "Game/File/Palette/ColourTable.h"

#include <wx/bitmap.h>

namespace TileUtilities {
	std::unique_ptr<wxBitmap> createWXBitmap(Tile * tile, ColourTable * colourTable);
}
