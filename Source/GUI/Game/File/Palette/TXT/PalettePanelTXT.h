#ifndef _PALETTE_PANEL_TXT_H_
#define _PALETTE_PANEL_TXT_H_

#include "Game/File/Palette/TXT/PaletteTXT.h"
#include "GUI/Game/File/Palette/PalettePanel.h"

class PalettePanelTXT final : public PalettePanel {
public:
	PalettePanelTXT(std::unique_ptr<PaletteTXT> palette, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~PalettePanelTXT() override;

	std::shared_ptr<const PaletteTXT> getPaletteTXT() const;
	std::shared_ptr<PaletteTXT> getPaletteTXT();

private:
	PalettePanelTXT(const PalettePanelTXT &) = delete;
	const PalettePanelTXT & operator = (const PalettePanelTXT &) = delete;
};

#endif // _PALETTE_PANEL_TXT_H_
