#include "PalettePanelACT.h"

#include <Utilities/StringUtilities.h>

#include <magic_enum/magic_enum.hpp>

PalettePanelACT::PalettePanelACT(std::unique_ptr<PaletteACT> palette, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style)
	: PalettePanel(std::move(palette), parent, windowID, position, size, style) {
	wxPanel * bytesPerColourPropertyPanel = new wxPanel(m_palettePropertiesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText * bytesPerColourLabel = new wxStaticText(bytesPerColourPropertyPanel, wxID_ANY, "Bytes Per Colour:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	bytesPerColourLabel->SetFont(bytesPerColourLabel->GetFont().MakeBold());
	wxStaticText * bytesPerColourText = new wxStaticText(bytesPerColourPropertyPanel, wxID_ANY, std::to_string(PaletteACT::BYTES_PER_COLOUR), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);

	wxPanel * endiannessPropertyPanel = new wxPanel(m_palettePropertiesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText * endiannessLabel = new wxStaticText(endiannessPropertyPanel, wxID_ANY, "Endianness:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	endiannessLabel->SetFont(endiannessLabel->GetFont().MakeBold());
	wxStaticText * endiannessText = new wxStaticText(endiannessPropertyPanel, wxID_ANY, Utilities::toCapitalCase(magic_enum::enum_name(PaletteACT::ENDIANNESS)), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);

	const int border = 5;

	wxFlexGridSizer * endiannessPropertySizer = new wxFlexGridSizer(2, 3, 3);
	endiannessPropertySizer->Add(endiannessLabel, 0, wxALIGN_CENTER_VERTICAL, border);
	endiannessPropertySizer->Add(endiannessText, 0, wxALIGN_CENTER_VERTICAL, border);
	endiannessPropertyPanel->SetSizer(endiannessPropertySizer);

	wxFlexGridSizer * bytesPerColourPropertySizer = new wxFlexGridSizer(2, 3, 3);
	bytesPerColourPropertySizer->Add(bytesPerColourLabel, 0, wxALIGN_CENTER_VERTICAL, border);
	bytesPerColourPropertySizer->Add(bytesPerColourText, 0, wxALIGN_CENTER_VERTICAL, border);
	bytesPerColourPropertyPanel->SetSizer(bytesPerColourPropertySizer);

	m_palettePropertiesSizer->Add(endiannessPropertyPanel, 0, wxLEFT | wxRIGHT, border);
	m_palettePropertiesSizer->Add(bytesPerColourPropertyPanel, 0, wxLEFT | wxRIGHT, border);
}

PalettePanelACT::~PalettePanelACT() { }

std::shared_ptr<const PaletteACT> PalettePanelACT::getPaletteACT() const {
	return std::dynamic_pointer_cast<const PaletteACT>(getGameFile());
}

std::shared_ptr<PaletteACT> PalettePanelACT::getPaletteACT() {
	return std::dynamic_pointer_cast<PaletteACT>(getGameFile());
}
