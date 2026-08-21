#ifndef _PALETTE_PANEL_H_
#define _PALETTE_PANEL_H_

#include "GUI/Game/File/GameFilePanel.h"
#include "GUI/Game/File/Palette/ColourTablePanel.h"

#include <boost/signals2.hpp>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/gbsizer.h>
#include <wx/wrapsizer.h>

class GameFile;
class Palette;
class ColourTable;
class MetadataPanel;

class PalettePanel : public GameFilePanel {
public:
	PalettePanel(std::unique_ptr<Palette> palette, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~PalettePanel() override;

	std::shared_ptr<const Palette> getPalette() const;
	std::shared_ptr<Palette> getPalette();
	void update() override;

	// GameFilePanel Virtuals
	std::string getPanelName() const override;

protected:
	wxPanel * m_palettePropertiesPanel;
	wxWrapSizer * m_palettePropertiesSizer;
	wxGridBagSizer * m_paletteInfoSizer;

private:
	wxStaticText * m_numberOfColourTablesText;
	wxStaticText * m_paletteSizeText;
	MetadataPanel * m_fileInfoPanel;
	wxBoxSizer * m_fileInfoBoxSizer;
	std::vector<ColourTablePanel *> m_colourTablePanels;
	wxScrolledWindow * m_colourTablesScrolledWindow;
	wxWrapSizer * m_colourTablesScrolledWindowSizer;

	PalettePanel(const PalettePanel &) = delete;
	const PalettePanel & operator = (const PalettePanel &) = delete;
};

#endif // _PALETTE_PANEL_H_
