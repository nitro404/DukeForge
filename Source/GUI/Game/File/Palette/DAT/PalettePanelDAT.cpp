#include "PalettePanelDAT.h"

PalettePanelDAT::PalettePanelDAT(std::unique_ptr<PaletteDAT> palette, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style)
	: PalettePanel(std::move(palette), parent, windowID, position, size, style)
	, m_numberOfShadeTablesText(nullptr)
	, m_numberOfSwapTablesText(nullptr) {
	const std::shared_ptr<const PaletteDAT> paletteDAT(getPaletteDAT());

	wxPanel * numberOfShadeTablesPropertyPanel = new wxPanel(m_palettePropertiesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText * numberOfShadeTablesLabel = new wxStaticText(numberOfShadeTablesPropertyPanel, wxID_ANY, "Number of Shade Tables:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	numberOfShadeTablesLabel->SetFont(numberOfShadeTablesLabel->GetFont().MakeBold());
	m_numberOfShadeTablesText = new wxStaticText(numberOfShadeTablesPropertyPanel, wxID_ANY, std::to_string(paletteDAT->numberOfShadeTables()), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);

	wxPanel * numberOfSwapTablesPropertyPanel = new wxPanel(m_palettePropertiesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText * numberOfSwapTablesLabel = new wxStaticText(numberOfSwapTablesPropertyPanel, wxID_ANY, "Number of Swap Tables:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	numberOfSwapTablesLabel->SetFont(numberOfSwapTablesLabel->GetFont().MakeBold());
	m_numberOfSwapTablesText = new wxStaticText(numberOfSwapTablesPropertyPanel, wxID_ANY, std::to_string(paletteDAT->numberOfSwapTables()), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);

	const int border = 5;

	wxFlexGridSizer * numberOfShadeTablesPropertySizer = new wxFlexGridSizer(2, 3, 3);
	numberOfShadeTablesPropertySizer->Add(numberOfShadeTablesLabel, 0, wxALIGN_CENTER_VERTICAL, border);
	numberOfShadeTablesPropertySizer->Add(m_numberOfShadeTablesText, 0, wxALIGN_CENTER_VERTICAL, border);
	numberOfShadeTablesPropertyPanel->SetSizer(numberOfShadeTablesPropertySizer);

	wxFlexGridSizer * numberOfSwapTablesPropertySizer = new wxFlexGridSizer(2, 3, 3);
	numberOfSwapTablesPropertySizer->Add(numberOfSwapTablesLabel, 0, wxALIGN_CENTER_VERTICAL, border);
	numberOfSwapTablesPropertySizer->Add(m_numberOfSwapTablesText, 0, wxALIGN_CENTER_VERTICAL, border);
	numberOfSwapTablesPropertyPanel->SetSizer(numberOfSwapTablesPropertySizer);

	m_palettePropertiesSizer->Add(numberOfShadeTablesPropertyPanel, 0, wxLEFT | wxRIGHT, border);
	m_palettePropertiesSizer->Add(numberOfSwapTablesPropertyPanel, 0, wxLEFT | wxRIGHT, border);
}

PalettePanelDAT::~PalettePanelDAT() { }

std::shared_ptr<const PaletteDAT> PalettePanelDAT::getPaletteDAT() const {
	return std::dynamic_pointer_cast<const PaletteDAT>(getGameFile());
}

std::shared_ptr<PaletteDAT> PalettePanelDAT::getPaletteDAT() {
	return std::dynamic_pointer_cast<PaletteDAT>(getGameFile());
}

void PalettePanelDAT::update() {
	std::shared_ptr<const PaletteDAT> palette(getPaletteDAT());

	if(palette == nullptr) {
		return;
	}

	m_numberOfShadeTablesText->SetLabel(std::to_string(palette->numberOfShadeTables()));
	m_numberOfSwapTablesText->SetLabel(std::to_string(palette->numberOfSwapTables()));
}
