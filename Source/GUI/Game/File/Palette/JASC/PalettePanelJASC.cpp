#include "PalettePanelJASC.h"

PalettePanelJASC::PalettePanelJASC(std::unique_ptr<PaletteJASC> palette, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style)
	: PalettePanel(std::move(palette), parent, windowID, position, size, style) {
	const std::shared_ptr<const PaletteJASC> paletteJASC(getPaletteJASC());

	wxPanel * versionPropertyPanel = new wxPanel(m_palettePropertiesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText * versionLabel = new wxStaticText(versionPropertyPanel, wxID_ANY, "Version:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	versionLabel->SetFont(versionLabel->GetFont().MakeBold());
	wxStaticText * versionText = new wxStaticText(versionPropertyPanel, wxID_ANY, paletteJASC->getVersion(), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);

	const int border = 5;

	wxFlexGridSizer * versionPropertySizer = new wxFlexGridSizer(2, 3, 3);
	versionPropertySizer->Add(versionLabel, 0, wxALIGN_CENTER_VERTICAL, border);
	versionPropertySizer->Add(versionText, 0, wxALIGN_CENTER_VERTICAL, border);
	versionPropertyPanel->SetSizer(versionPropertySizer);

	m_palettePropertiesSizer->Add(versionPropertyPanel, 0, wxLEFT | wxRIGHT, border);
}

PalettePanelJASC::~PalettePanelJASC() { }

std::shared_ptr<const PaletteJASC> PalettePanelJASC::getPaletteJASC() const {
	return std::dynamic_pointer_cast<const PaletteJASC>(getGameFile());
}

std::shared_ptr<PaletteJASC> PalettePanelJASC::getPaletteJASC() {
	return std::dynamic_pointer_cast<PaletteJASC>(getGameFile());
}
