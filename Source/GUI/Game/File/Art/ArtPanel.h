#ifndef _ART_PANEL_H_
#define _ART_PANEL_H_

#include "GUI/Game/File/GameFilePanel.h"
#include "GUI/Game/File/Art/TilePanel.h"

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

class Art;
class GameFile;
class MetadataPanel;
class Tile;

class ArtPanel final : public GameFilePanel {
public:
	ArtPanel(std::unique_ptr<Art> art, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~ArtPanel() override;

	std::shared_ptr<const Art> getArt() const;
	std::shared_ptr<Art> getArt();
	void update() override;

	// GameFilePanel Virtuals
	std::string getPanelName() const override;

protected:
	wxPanel * m_artPropertiesPanel;
	wxWrapSizer * m_artPropertiesSizer;
	wxGridBagSizer * m_artInfoSizer;

private:
	wxStaticText * m_numberOfTilesText;
	MetadataPanel * m_fileInfoPanel;
	wxBoxSizer * m_fileInfoBoxSizer;
	std::vector<TilePanel *> m_tilePanels;
	wxScrolledWindow * m_tilesScrolledWindow;
	wxWrapSizer * m_tilesScrolledWindowSizer;

	ArtPanel(const ArtPanel &) = delete;
	const ArtPanel & operator = (const ArtPanel &) = delete;
};

#endif // _ART_PANEL_H_
