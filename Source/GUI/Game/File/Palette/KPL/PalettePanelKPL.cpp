#include "PalettePanelKPL.h"

PalettePanelKPL::PalettePanelKPL(std::unique_ptr<PaletteKPL> palette, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style)
	: PalettePanel(std::move(palette), parent, windowID, position, size, style) {
	const std::shared_ptr<const PaletteKPL> paletteKPL(getPaletteKPL());

	wxPanel * readOnlyPropertyPanel = new wxPanel(m_palettePropertiesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText * readOnlyLabel = new wxStaticText(readOnlyPropertyPanel, wxID_ANY, "Read Only:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	readOnlyLabel->SetFont(readOnlyLabel->GetFont().MakeBold());
	wxStaticText * readOnlyText = new wxStaticText(readOnlyPropertyPanel, wxID_ANY, paletteKPL->isReadOnly() ? "Yes" : "No", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);

	const int border = 5;

	wxFlexGridSizer * readOnlyPropertySizer = new wxFlexGridSizer(2, 3, 3);
	readOnlyPropertySizer->Add(readOnlyLabel, 0, wxALIGN_CENTER_VERTICAL, border);
	readOnlyPropertySizer->Add(readOnlyText, 0, wxALIGN_CENTER_VERTICAL, border);
	readOnlyPropertyPanel->SetSizer(readOnlyPropertySizer);

	m_palettePropertiesSizer->Add(readOnlyPropertyPanel, 0, wxLEFT | wxRIGHT, border);
}

PalettePanelKPL::~PalettePanelKPL() { }

std::shared_ptr<const PaletteKPL> PalettePanelKPL::getPaletteKPL() const {
	return std::dynamic_pointer_cast<const PaletteKPL>(getGameFile());
}

std::shared_ptr<PaletteKPL> PalettePanelKPL::getPaletteKPL() {
	return std::dynamic_pointer_cast<PaletteKPL>(getGameFile());
}
