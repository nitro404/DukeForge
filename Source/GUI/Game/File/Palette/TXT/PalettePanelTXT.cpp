#include "PalettePanelTXT.h"

PalettePanelTXT::PalettePanelTXT(std::unique_ptr<PaletteTXT> palette, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style)
	: PalettePanel(std::move(palette), parent, windowID, position, size, style) { }

PalettePanelTXT::~PalettePanelTXT() { }

std::shared_ptr<const PaletteTXT> PalettePanelTXT::getPaletteTXT() const {
	return std::dynamic_pointer_cast<const PaletteTXT>(getGameFile());
}

std::shared_ptr<PaletteTXT> PalettePanelTXT::getPaletteTXT() {
	return std::dynamic_pointer_cast<PaletteTXT>(getGameFile());
}
