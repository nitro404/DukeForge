#ifndef _PALETTE_PANEL_PAL_H_
#define _PALETTE_PANEL_PAL_H_

#include "Game/File/Palette/PAL/PalettePAL.h"
#include "GUI/Game/File/Palette/PalettePanel.h"

class PalettePanelPAL final : public PalettePanel {
public:
	PalettePanelPAL(std::unique_ptr<PalettePAL> palette, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~PalettePanelPAL() override;

	std::shared_ptr<const PalettePAL> getPalettePAL() const;
	std::shared_ptr<PalettePAL> getPalettePAL();

private:
	PalettePanelPAL(const PalettePanelPAL &) = delete;
	const PalettePanelPAL & operator = (const PalettePanelPAL &) = delete;
};

#endif // _PALETTE_PANEL_PAL_H_
