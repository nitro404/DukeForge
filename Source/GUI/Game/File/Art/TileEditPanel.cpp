#include "TileEditPanel.h"

#include "Game/File/Art/Tile.h"
#include "GUI/Game/File/Art/TileUtilities.h"
#include "GUI/WXUtilities.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <wx/colordlg.h>
#include <wx/colourdata.h>
#include <wx/dcbuffer.h>
#include <wx/brush.h>

static constexpr uint8_t PIXEL_SCALE = 8;

TileEditPanel::TileEditPanel(std::shared_ptr<Tile> tile, std::shared_ptr<ColourTable> colourTable, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style)
	: wxPanel(parent, windowID, position, size, style)
	, m_tile(tile)
	, m_colourTable(colourTable)
	, m_yOffset(0) {
	wxASSERT(wxIsMainThread());

	Freeze();

	SetBackgroundColour(parent->GetBackgroundColour());
	SetBackgroundStyle(wxBG_STYLE_PAINT);

	update();

	Thaw();

	Bind(wxEVT_PAINT, &TileEditPanel::onPaint, this);
}

TileEditPanel::~TileEditPanel() {
	m_tileModifiedConnection.disconnect();
}

std::unique_ptr<wxBitmap> TileEditPanel::createWXBitmap() const {
	return TileUtilities::createWXBitmap(m_tile.get(), m_colourTable.get());
}

void TileEditPanel::update() {
	wxASSERT(wxIsMainThread());

	m_bitmap = createWXBitmap();

	if(m_bitmap != nullptr) {
		SetMinSize(wxSize(m_bitmap->GetWidth(), m_bitmap->GetHeight()));
	}
}

void TileEditPanel::onTileModified(const Tile & tile) {
	update();
}

void TileEditPanel::onPaint(wxPaintEvent & event) {
	wxASSERT(wxIsMainThread());

	wxAutoBufferedPaintDC paintContext(this);

	paintContext.SetBackground(wxBrush(GetBackgroundColour()));
	paintContext.Clear();

	if (m_bitmap != nullptr) {
		paintContext.DrawBitmap(*m_bitmap, 0, m_yOffset);
	}
}
