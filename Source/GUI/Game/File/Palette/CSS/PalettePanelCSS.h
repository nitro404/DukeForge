#ifndef _PALETTE_PANEL_CSS_H_
#define _PALETTE_PANEL_CSS_H_

#include "Game/File/Palette/CSS/PaletteCSS.h"
#include "GUI/Game/File/Palette/PalettePanel.h"

class PalettePanelCSS final : public PalettePanel {
public:
	PalettePanelCSS(std::unique_ptr<PaletteCSS> palette, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~PalettePanelCSS() override;

	std::shared_ptr<const PaletteCSS> getPaletteCSS() const;
	std::shared_ptr<PaletteCSS> getPaletteCSS();

private:
	PalettePanelCSS(const PalettePanelCSS &) = delete;
	const PalettePanelCSS & operator = (const PalettePanelCSS &) = delete;
};

#endif // _PALETTE_PANEL_CSS_H_
