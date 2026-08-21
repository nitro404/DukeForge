#ifndef _PALETTE_PANEL_GPL_H_
#define _PALETTE_PANEL_GPL_H_

#include "Game/File/Palette/GPL/PaletteGPL.h"
#include "GUI/Game/File/Palette/PalettePanel.h"

class PalettePanelGPL final : public PalettePanel {
public:
	PalettePanelGPL(std::unique_ptr<PaletteGPL> palette, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~PalettePanelGPL() override;

	std::shared_ptr<const PaletteGPL> getPaletteGPL() const;
	std::shared_ptr<PaletteGPL> getPaletteGPL();

private:
	PalettePanelGPL(const PalettePanelGPL &) = delete;
	const PalettePanelGPL & operator = (const PalettePanelGPL &) = delete;
};

#endif // _PALETTE_PANEL_GPL_H_
