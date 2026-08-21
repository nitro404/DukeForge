#include "PalettePanelCSS.h"

PalettePanelCSS::PalettePanelCSS(std::unique_ptr<PaletteCSS> palette, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style)
	: PalettePanel(std::move(palette), parent, windowID, position, size, style) { }

PalettePanelCSS::~PalettePanelCSS() { }

std::shared_ptr<const PaletteCSS> PalettePanelCSS::getPaletteCSS() const {
	return std::dynamic_pointer_cast<const PaletteCSS>(getGameFile());
}

std::shared_ptr<PaletteCSS> PalettePanelCSS::getPaletteCSS() {
	return std::dynamic_pointer_cast<PaletteCSS>(getGameFile());
}
