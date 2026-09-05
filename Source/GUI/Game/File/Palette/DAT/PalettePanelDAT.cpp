#include "PalettePanelDAT.h"

#include <Utilities/StringUtilities.h>

#include <magic_enum/magic_enum.hpp>

PalettePanelDAT::PalettePanelDAT(std::unique_ptr<PaletteDAT> palette, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style)
	: PalettePanel(std::move(palette), parent, windowID, position, size, style)
	, m_numberOfShadeTablesText(nullptr)
	, m_numberOfSwapTablesText(nullptr) {
	const std::shared_ptr<const PaletteDAT> paletteDAT(getPaletteDAT());

	Freeze();

	wxPanel * numberOfShadeTablesPropertyPanel = new wxPanel(m_palettePropertiesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText * numberOfShadeTablesLabel = new wxStaticText(numberOfShadeTablesPropertyPanel, wxID_ANY, "Number of Shade Tables:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	numberOfShadeTablesLabel->SetFont(numberOfShadeTablesLabel->GetFont().MakeBold());
	m_numberOfShadeTablesText = new wxStaticText(numberOfShadeTablesPropertyPanel, wxID_ANY, std::to_string(paletteDAT->numberOfShadeTables()), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);

	wxPanel * numberOfSwapTablesPropertyPanel = new wxPanel(m_palettePropertiesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText * numberOfSwapTablesLabel = new wxStaticText(numberOfSwapTablesPropertyPanel, wxID_ANY, "Number of Swap Tables:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	numberOfSwapTablesLabel->SetFont(numberOfSwapTablesLabel->GetFont().MakeBold());
	m_numberOfSwapTablesText = new wxStaticText(numberOfSwapTablesPropertyPanel, wxID_ANY, std::to_string(paletteDAT->numberOfSwapTables()), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);

	wxPanel * bytesPerColourPropertyPanel = new wxPanel(m_palettePropertiesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText * bytesPerColourLabel = new wxStaticText(bytesPerColourPropertyPanel, wxID_ANY, "Bytes Per Colour:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	bytesPerColourLabel->SetFont(bytesPerColourLabel->GetFont().MakeBold());
	wxStaticText * bytesPerColourText = new wxStaticText(bytesPerColourPropertyPanel, wxID_ANY, std::to_string(PaletteDAT::BYTES_PER_COLOUR), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);

	wxPanel * endiannessPropertyPanel = new wxPanel(m_palettePropertiesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText * endiannessLabel = new wxStaticText(endiannessPropertyPanel, wxID_ANY, "Endianness:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	endiannessLabel->SetFont(endiannessLabel->GetFont().MakeBold());
	wxStaticText * endiannessText = new wxStaticText(endiannessPropertyPanel, wxID_ANY, Utilities::toCapitalCase(magic_enum::enum_name(PaletteDAT::ENDIANNESS)), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);

	const int border = 5;

	wxFlexGridSizer * numberOfShadeTablesPropertySizer = new wxFlexGridSizer(2, 3, 3);
	numberOfShadeTablesPropertySizer->Add(numberOfShadeTablesLabel, 0, wxALIGN_CENTER_VERTICAL, border);
	numberOfShadeTablesPropertySizer->Add(m_numberOfShadeTablesText, 0, wxALIGN_CENTER_VERTICAL, border);
	numberOfShadeTablesPropertyPanel->SetSizer(numberOfShadeTablesPropertySizer);

	wxFlexGridSizer * numberOfSwapTablesPropertySizer = new wxFlexGridSizer(2, 3, 3);
	numberOfSwapTablesPropertySizer->Add(numberOfSwapTablesLabel, 0, wxALIGN_CENTER_VERTICAL, border);
	numberOfSwapTablesPropertySizer->Add(m_numberOfSwapTablesText, 0, wxALIGN_CENTER_VERTICAL, border);
	numberOfSwapTablesPropertyPanel->SetSizer(numberOfSwapTablesPropertySizer);

	wxFlexGridSizer * endiannessPropertySizer = new wxFlexGridSizer(2, 3, 3);
	endiannessPropertySizer->Add(endiannessLabel, 0, wxALIGN_CENTER_VERTICAL, border);
	endiannessPropertySizer->Add(endiannessText, 0, wxALIGN_CENTER_VERTICAL, border);
	endiannessPropertyPanel->SetSizer(endiannessPropertySizer);

	wxFlexGridSizer * bytesPerColourPropertySizer = new wxFlexGridSizer(2, 3, 3);
	bytesPerColourPropertySizer->Add(bytesPerColourLabel, 0, wxALIGN_CENTER_VERTICAL, border);
	bytesPerColourPropertySizer->Add(bytesPerColourText, 0, wxALIGN_CENTER_VERTICAL, border);
	bytesPerColourPropertyPanel->SetSizer(bytesPerColourPropertySizer);

	m_palettePropertiesSizer->Add(numberOfShadeTablesPropertyPanel, 0, wxLEFT | wxRIGHT, border);
	m_palettePropertiesSizer->Add(numberOfSwapTablesPropertyPanel, 0, wxLEFT | wxRIGHT, border);
	m_palettePropertiesSizer->Add(endiannessPropertyPanel, 0, wxLEFT | wxRIGHT, border);
	m_palettePropertiesSizer->Add(bytesPerColourPropertyPanel, 0, wxLEFT | wxRIGHT, border);

	Thaw();
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
