#include "TilePanel.h"

#include "Game/File/Art/Tile.h"
#include "GUI/WXUtilities.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <wx/colordlg.h>
#include <wx/colourdata.h>
#include <wx/dcbuffer.h>
#include <wx/brush.h>

static constexpr uint8_t PIXEL_SCALE = 8;

TilePanel::TilePanel(std::shared_ptr<Tile> tile, std::shared_ptr<ColourTable> colourTable, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style)
	: wxPanel(parent, windowID, position, size, style)
	, m_tile(tile)
	, m_colourTable(colourTable)
	, m_yOffset(0) {
	wxASSERT(wxIsMainThread());

	SetBackgroundColour(parent->GetBackgroundColour());
	SetBackgroundStyle(wxBG_STYLE_PAINT);

// TODO: split onto two lines, one for number, one for name?
	m_tileNameLabel = new wxStaticText(this, wxID_ANY, m_tile->getDefaultName(), wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER);
	m_tileNameLabel->SetFont(m_tileNameLabel->GetFont().MakeBold().MakeLarger());

	// Note: For some reason ampersands are not rendered if you assign the text contents upon creation rather than through the set label text function
	update();

	Bind(wxEVT_LEFT_DOWN, &TilePanel::onMouseDown, this);
	Bind(wxEVT_PAINT, &TilePanel::onPaint, this);
}

TilePanel::~TilePanel() {
	m_tileModifiedConnection.disconnect();
}

std::unique_ptr<wxBitmap> TilePanel::createWXBitmap() const {
	return createWXBitmap(m_tile.get(), m_colourTable.get());
}

std::unique_ptr<wxBitmap> TilePanel::createWXBitmap(Tile * tile, ColourTable * colourTable) {
	if(tile == nullptr || tile->isEmpty()) {
		return nullptr;
	}

	const uint16_t width = tile->getWidth();
	const uint16_t height = tile->getHeight();
	const std::vector<uint8_t> & pixels = tile->getPixels();
	wxImage tileImage(static_cast<int>(width), static_cast<int>(height), false);
	uint8_t * tileImageData = tileImage.GetData();

	for(size_t x = 0; x < width; ++x) {
		for(size_t y = 0; y < height; ++y) {
			const uint8_t pixel = pixels[(x * height) + y];
			const Colour & colour = colourTable->getColour(pixel);
			const size_t destinationPixelIndex = ((y * width) + x) * 3;

			tileImageData[destinationPixelIndex + 0] = colour.r;
			tileImageData[destinationPixelIndex + 1] = colour.g;
			tileImageData[destinationPixelIndex + 2] = colour.b;
		}
	}

	return std::make_unique<wxBitmap>(std::move(tileImage));
}

void TilePanel::update() {
	wxASSERT(wxIsMainThread());

	// TODO: leading zeroes:

	m_tileNameLabel->SetLabelText(wxString::FromUTF8(fmt::format("{}{}", m_tile->getNumber(), m_tile->hasDefaultName() ? fmt::format(" ({})", m_tile->getDefaultName()) : "")));

	m_bitmap = createWXBitmap();

	const wxSize bestSize(m_tileNameLabel->GetBestSize());

	m_yOffset = bestSize.y;

	if(m_bitmap != nullptr) {
		SetMinSize(wxSize(std::max(bestSize.x, m_bitmap->GetWidth()), m_bitmap->GetHeight() + bestSize.y));
	}
}

void TilePanel::onTileModified(const Tile & tile) {
	update();
}

void TilePanel::onPaint(wxPaintEvent & event) {
	wxASSERT(wxIsMainThread());

	wxAutoBufferedPaintDC paintContext(this);

	paintContext.SetBackground(wxBrush(GetBackgroundColour()));
	paintContext.Clear();

	if (m_bitmap != nullptr) {
		paintContext.DrawBitmap(*m_bitmap, 0, m_yOffset);
	}
}

void TilePanel::onMouseDown(wxMouseEvent & event) {
	// TODO
}
