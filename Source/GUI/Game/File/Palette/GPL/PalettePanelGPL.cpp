#include "PalettePanelGPL.h"

PalettePanelGPL::PalettePanelGPL(std::unique_ptr<PaletteGPL> palette, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style)
	: PalettePanel(std::move(palette), parent, windowID, position, size, style) { }

PalettePanelGPL::~PalettePanelGPL() { }

std::shared_ptr<const PaletteGPL> PalettePanelGPL::getPaletteGPL() const {
	return std::dynamic_pointer_cast<const PaletteGPL>(getGameFile());
}

std::shared_ptr<PaletteGPL> PalettePanelGPL::getPaletteGPL() {
	return std::dynamic_pointer_cast<PaletteGPL>(getGameFile());
}
