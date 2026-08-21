#ifndef _PALETTE_PANEL_ACT_H_
#define _PALETTE_PANEL_ACT_H_

#include "Game/File/Palette/ACT/PaletteACT.h"
#include "GUI/Game/File/Palette/PalettePanel.h"

class PalettePanelACT final : public PalettePanel {
public:
	PalettePanelACT(std::unique_ptr<PaletteACT> palette, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~PalettePanelACT() override;

	std::shared_ptr<const PaletteACT> getPaletteACT() const;
	std::shared_ptr<PaletteACT> getPaletteACT();

private:
	PalettePanelACT(const PalettePanelACT &) = delete;
	const PalettePanelACT & operator = (const PalettePanelACT &) = delete;
};

#endif // _PALETTE_PANEL_ACT_H_
