#ifndef _TILE_EDIT_PANEL_H_
#define _TILE_EDIT_PANEL_H_

#include "Game/File/Art/Tile.h"
#include "Game/File/Palette/ColourTable.h"

#include <boost/signals2.hpp>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <memory>

class TileEditPanel final : public wxPanel {
public:
	TileEditPanel(std::shared_ptr<Tile> tile, std::shared_ptr<ColourTable> colourTable, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~TileEditPanel() override;

private:
	std::unique_ptr<wxBitmap> createWXBitmap() const;
	void update();
	void onTileModified(const Tile & tile);
	void onPaint(wxPaintEvent & event);

	std::shared_ptr<Tile> m_tile;
	std::shared_ptr<ColourTable> m_colourTable;
	uint32_t m_yOffset;
	std::unique_ptr<wxBitmap> m_bitmap;
	boost::signals2::connection m_tileModifiedConnection;

	TileEditPanel(const TileEditPanel &) = delete;
	const TileEditPanel & operator = (const TileEditPanel &) = delete;
};

#endif // _TILE_EDIT_PANEL_H_
