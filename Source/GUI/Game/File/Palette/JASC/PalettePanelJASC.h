#ifndef _PALETTE_PANEL_JASC_H_
#define _PALETTE_PANEL_JASC_H_

#include "Game/File/Palette/JASC/PaletteJASC.h"
#include "GUI/Game/File/Palette/PalettePanel.h"

class PalettePanelJASC final : public PalettePanel {
public:
	PalettePanelJASC(std::unique_ptr<PaletteJASC> palette, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~PalettePanelJASC() override;

	std::shared_ptr<const PaletteJASC> getPaletteJASC() const;
	std::shared_ptr<PaletteJASC> getPaletteJASC();

private:
	PalettePanelJASC(const PalettePanelJASC &) = delete;
	const PalettePanelJASC & operator = (const PalettePanelJASC &) = delete;
};

#endif // _PALETTE_PANEL_JASC_H_
