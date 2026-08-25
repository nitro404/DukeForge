#ifndef _TILE_PANEL_H_
#define _TILE_PANEL_H_

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

class TilePanel final : public wxPanel {
public:
	TilePanel(std::shared_ptr<Tile> tile, std::shared_ptr<ColourTable> colourTable, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~TilePanel() override;

private:
	std::unique_ptr<wxBitmap> createWXBitmap() const;
	void update();
	void onTileModified(const Tile & tile);
	void onPaint(wxPaintEvent & event);
	void onMouseDown(wxMouseEvent & event);

	wxStaticText * m_tileNameLabel;
	std::shared_ptr<Tile> m_tile;
	std::shared_ptr<ColourTable> m_colourTable;
	uint32_t m_yOffset;
	std::unique_ptr<wxBitmap> m_bitmap;
	boost::signals2::connection m_tileModifiedConnection;

	TilePanel(const TilePanel &) = delete;
	const TilePanel & operator = (const TilePanel &) = delete;
};

#endif // _TILE_PANEL_H_
