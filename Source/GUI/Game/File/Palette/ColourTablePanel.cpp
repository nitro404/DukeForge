#include "ColourTablePanel.h"

#include "Game/File/Palette/Palette.h"
#include "GUI/WXUtilities.h"

#include <fmt/core.h>
#include <spdlog/spdlog.h>
#include <wx/colordlg.h>
#include <wx/colourdata.h>
#include <wx/dcbuffer.h>
#include <wx/brush.h>

static constexpr uint8_t PIXEL_SCALE = 24;

ColourTablePanel::ColourTablePanel(std::shared_ptr<ColourTable> colourTable, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style)
	: wxPanel(parent, windowID, position, size, style)
	, m_colourTable(colourTable) {
	wxASSERT(wxIsMainThread());

	SetBackgroundColour(parent->GetBackgroundColour());
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetMinSize(wxSize(COLOUR_TABLE_RENDER_WIDTH * PIXEL_SCALE, COLOUR_TABLE_RENDER_HEIGHT * PIXEL_SCALE));

	m_colourTableNameLabel = new wxStaticText(this, wxID_ANY, m_colourTable->getName(), wxDefaultPosition, wxSize(COLOUR_TABLE_RENDER_WIDTH * PIXEL_SCALE, -1), wxALIGN_CENTER);
	m_colourTableNameLabel->SetFont(m_colourTableNameLabel->GetFont().MakeBold().MakeLarger().MakeLarger());

	// Note: For some reason ampersands are not rendered if you assign the text contents upon creation rather than through the set label text function
	updateColourTableNameLabel();

	Bind(wxEVT_LEFT_DOWN, &ColourTablePanel::onMouseDown, this);
	Bind(wxEVT_PAINT, &ColourTablePanel::onPaint, this);
}

ColourTablePanel::~ColourTablePanel() {
	m_colourTableModifiedConnection.disconnect();
}

size_t ColourTablePanel::getVerticalOffset() const {
	wxASSERT(wxIsMainThread());

	if(!m_colourTable->hasName()) {
		return 0u;
	}

	return m_colourTableNameLabel->GetSize().GetHeight();
}

void ColourTablePanel::updateColourTableNameLabel() {
	wxASSERT(wxIsMainThread());

	m_colourTableNameLabel->SetLabelText(wxString::FromUTF8(m_colourTable->getName()));
	m_colourTableNameLabel->Show(m_colourTable->hasName());

	SetMinSize(wxSize(COLOUR_TABLE_RENDER_WIDTH * PIXEL_SCALE, (COLOUR_TABLE_RENDER_HEIGHT * PIXEL_SCALE) + getVerticalOffset()));
}

void ColourTablePanel::onColourTableModified(const ColourTable & colourTable) {
	updateColourTableNameLabel();
}

void ColourTablePanel::onPaint(wxPaintEvent & event) {
	wxAutoBufferedPaintDC paintContext(this);

	paintContext.SetBackground(wxBrush(GetBackgroundColour()));
	paintContext.Clear();

	for(size_t i = 0; i < m_colourTable->numberOfColours(); i++) {
		paintContext.SetBrush(wxBrush(WXUtilities::createWXColour(m_colourTable->getColour(i))));
		paintContext.SetPen(*wxTRANSPARENT_PEN);
		paintContext.DrawRectangle((i % COLOUR_TABLE_RENDER_WIDTH) * PIXEL_SCALE, ((i / COLOUR_TABLE_RENDER_WIDTH) * PIXEL_SCALE) + getVerticalOffset(), PIXEL_SCALE, PIXEL_SCALE);
	}
}

void ColourTablePanel::onMouseDown(wxMouseEvent & event) {
	wxPoint position(event.GetPosition());

	if(position.x < 0 || position.y < getVerticalOffset() || position.x > COLOUR_TABLE_RENDER_WIDTH * PIXEL_SCALE || position.y > (COLOUR_TABLE_RENDER_HEIGHT * PIXEL_SCALE) + getVerticalOffset()) {
		return;
	}

	const uint8_t pixelIndex = static_cast<uint8_t>((position.x / PIXEL_SCALE) + (((position.y - getVerticalOffset()) / PIXEL_SCALE) * COLOUR_TABLE_RENDER_WIDTH));

	if(pixelIndex >= m_colourTable->numberOfColours()) {
		return;
	}

	spdlog::debug("Selected colour table pixel index #{}.", pixelIndex);

	wxColourData colourData;
	colourData.SetColour(WXUtilities::createWXColour(m_colourTable->getColour(pixelIndex)));
	wxColourDialog pixelColourDialog(this, &colourData);

	if(pixelColourDialog.ShowModal() != wxID_OK) {
		return;
	}

	const Colour newColour(WXUtilities::createColour(pixelColourDialog.GetColourData().GetColour()));

	if(m_colourTable->setColour(pixelIndex, newColour)) {
		spdlog::info("Successfully set colour table pixel index #{} to {}!", pixelIndex, newColour.toString());
	}
	else {
		WXUtilities::showErrorMessage(fmt::format("Failed to set colour tabel pixel index #{} to {}.", pixelIndex, newColour.toString()), "Pixel Colour Update Failed", this);
	}
}
