#include "PalettePanel.h"

#include "Game/File/GameFile.h"
#include "Game/File/GameFileFactoryRegistry.h"
#include "Game/File/Palette/Palette.h"
#include "Game/File/Palette/ColourTable.h"
#include "GUI/MetadataPanel.h"
#include "GUI/WXUtilities.h"

#include <Utilities/FileUtilities.h>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

PalettePanel::PalettePanel(std::unique_ptr<Palette> palette, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style)
	: GameFilePanel(std::move(palette), parent, windowID, position, size, style, "Palette")
	, m_palettePropertiesPanel(nullptr)
	, m_palettePropertiesSizer(nullptr)
	, m_paletteInfoSizer(nullptr)
	, m_numberOfColourTablesText(nullptr)
	, m_paletteSizeText(nullptr)
	, m_fileInfoPanel(nullptr)
	, m_fileInfoBoxSizer(nullptr)
	, m_colourTablesScrolledWindow(nullptr)
	, m_colourTablesScrolledWindowSizer(nullptr) {
	wxASSERT(wxIsMainThread());

	Freeze();

	m_palettePropertiesPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxPanel * numberOfColourTablesPropertyPanel = new wxPanel(m_palettePropertiesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText * numberOfColourTablesLabel = new wxStaticText(numberOfColourTablesPropertyPanel, wxID_ANY, "Number of Colour Tables:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	numberOfColourTablesLabel->SetFont(numberOfColourTablesLabel->GetFont().MakeBold());
	m_numberOfColourTablesText = new wxStaticText(numberOfColourTablesPropertyPanel, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);

	wxPanel * paletteSizePropertyPanel = new wxPanel(m_palettePropertiesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText * paletteSizeLabel = new wxStaticText(paletteSizePropertyPanel, wxID_ANY, "Palette Size:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	paletteSizeLabel->SetFont(paletteSizeLabel->GetFont().MakeBold());
	m_paletteSizeText = new wxStaticText(paletteSizePropertyPanel, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);

	m_colourTablesScrolledWindow = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_colourTablesScrolledWindow->SetScrollRate(0, 10);
	m_colourTablesScrolledWindow->SetBackgroundColour(GetBackgroundColour());

	std::vector<std::shared_ptr<ColourTable>> colourTables(getPalette()->getAllColourTables());

	for(std::shared_ptr<ColourTable> & colourTable : colourTables) {
		m_colourTablePanels.push_back(new ColourTablePanel(colourTable, m_colourTablesScrolledWindow));
	}

	const int border = 5;

	wxFlexGridSizer * numberOfColourTablesPropertySizer = new wxFlexGridSizer(2, 3, 3);
	numberOfColourTablesPropertySizer->Add(numberOfColourTablesLabel, 0, wxALIGN_CENTER_VERTICAL, border);
	numberOfColourTablesPropertySizer->Add(m_numberOfColourTablesText, 0, wxALIGN_CENTER_VERTICAL, border);
	numberOfColourTablesPropertyPanel->SetSizer(numberOfColourTablesPropertySizer);

	wxFlexGridSizer * paletteSizePropertySizer = new wxFlexGridSizer(2, 3, 3);
	paletteSizePropertySizer->Add(paletteSizeLabel, 0, wxALIGN_CENTER_VERTICAL, border);
	paletteSizePropertySizer->Add(m_paletteSizeText, 0, wxALIGN_CENTER_VERTICAL, border);
	paletteSizePropertyPanel->SetSizer(paletteSizePropertySizer);

	m_palettePropertiesSizer = new wxWrapSizer(wxHORIZONTAL);
	m_palettePropertiesSizer->Add(numberOfColourTablesPropertyPanel, 0, wxLEFT | wxRIGHT, border);
	m_palettePropertiesSizer->Add(paletteSizePropertyPanel, 0, wxLEFT | wxRIGHT, border);
	m_palettePropertiesPanel->SetSizer(m_palettePropertiesSizer);

	m_paletteInfoSizer = new wxGridBagSizer(border, border);
	m_paletteInfoSizer->Add(m_palettePropertiesPanel, wxGBPosition(0, 0), wxGBSpan(1, 1), wxEXPAND | wxHORIZONTAL, border);
	m_paletteInfoSizer->Add(m_colourTablesScrolledWindow, wxGBPosition(1, 0), wxGBSpan(1, 1), wxEXPAND | wxHORIZONTAL | wxVERTICAL, border);
	m_paletteInfoSizer->AddGrowableRow(1, 1);
	m_paletteInfoSizer->AddGrowableCol(0, 1);

	m_colourTablesScrolledWindowSizer = new wxWrapSizer(wxHORIZONTAL);
	for(ColourTablePanel * colourTablePanel : m_colourTablePanels) {
		m_colourTablesScrolledWindowSizer->Add(colourTablePanel, 0, wxALL, border);
	}
	m_colourTablesScrolledWindow->SetSizer(m_colourTablesScrolledWindowSizer);

	SetSizer(m_paletteInfoSizer);

	update();

	Thaw();
}

PalettePanel::~PalettePanel() { }

std::string PalettePanel::getPanelName() const {
	std::string_view fileName(getGameFile()->getFileName());

	return fileName.empty() ? fmt::format("NEW {} PALETTE *", getGameFile()->getDefaultFileFormatExtension()) : fmt::format("{}{}", fileName, getGameFile()->isModified() ? " *" : "");
}

std::shared_ptr<const Palette> PalettePanel::getPalette() const {
	return std::dynamic_pointer_cast<const Palette>(getGameFile());
}

std::shared_ptr<Palette> PalettePanel::getPalette() {
	return std::dynamic_pointer_cast<Palette>(getGameFile());
}

void PalettePanel::update() {
	wxASSERT(wxIsMainThread());

	std::shared_ptr<const Palette> palette(getPalette());

	if(palette == nullptr) {
		return;
	}

	m_numberOfColourTablesText->SetLabel(std::to_string(palette->numberOfColourTables()));
	m_paletteSizeText->SetLabel(Utilities::fileSizeToString(palette->getSizeInBytes()));

	m_palettePropertiesSizer->Layout();
}
