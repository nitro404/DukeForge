#ifndef _PALETTE_PANEL_KPL_H_
#define _PALETTE_PANEL_KPL_H_

#include "Game/File/Palette/KPL/PaletteKPL.h"
#include "GUI/Game/File/Palette/PalettePanel.h"

class PalettePanelKPL final : public PalettePanel {
public:
	PalettePanelKPL(std::unique_ptr<PaletteKPL> palette, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~PalettePanelKPL() override;

	std::shared_ptr<const PaletteKPL> getPaletteKPL() const;
	std::shared_ptr<PaletteKPL> getPaletteKPL();

private:
	PalettePanelKPL(const PalettePanelKPL &) = delete;
	const PalettePanelKPL & operator = (const PalettePanelKPL &) = delete;
};

#endif // _PALETTE_PANEL_KPL_H_
