#ifndef _PALETTE_PANEL_DAT_H_
#define _PALETTE_PANEL_DAT_H_

#include "Game/File/Palette/DAT/PaletteDAT.h"
#include "GUI/Game/File/Palette/PalettePanel.h"

class PalettePanelDAT final : public PalettePanel {
public:
	PalettePanelDAT(std::unique_ptr<PaletteDAT> palette, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~PalettePanelDAT() override;

	std::shared_ptr<const PaletteDAT> getPaletteDAT() const;
	std::shared_ptr<PaletteDAT> getPaletteDAT();

	// PalettePanel Virtuals
	void update() override;

private:
	PalettePanelDAT(const PalettePanelDAT &) = delete;
	const PalettePanelDAT & operator = (const PalettePanelDAT &) = delete;

	wxStaticText * m_numberOfShadeTablesText;
	wxStaticText * m_numberOfSwapTablesText;
};

#endif // _PALETTE_PANEL_DAT_H_
