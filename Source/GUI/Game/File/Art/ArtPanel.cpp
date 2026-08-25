#include "ArtPanel.h"

#include "Game/File/Art/Art.h"
#include "Game/File/Art/Tile.h"
#include "Game/File/GameFile.h"
#include "Game/File/GameFileFactoryRegistry.h"
#include "Game/File/Palette/DAT/PaletteDAT.h"
#include "GUI/MetadataPanel.h"
#include "GUI/WXUtilities.h"

#include <Utilities/FileUtilities.h>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

ArtPanel::ArtPanel(std::unique_ptr<Art> art, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style)
	: GameFilePanel(std::move(art), parent, windowID, position, size, style, "Art")
	, m_artPropertiesPanel(nullptr)
	, m_artPropertiesSizer(nullptr)
	, m_artInfoSizer(nullptr)
	, m_numberOfTilesText(nullptr)
	, m_fileInfoPanel(nullptr)
	, m_fileInfoBoxSizer(nullptr)
	, m_tilesScrolledWindow(nullptr)
	, m_tilesScrolledWindowSizer(nullptr) {
	wxASSERT(wxIsMainThread());

	m_artPropertiesPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	// TODO:
	// Version
	// Art Number
	// Number of Empty Tiles
	// Number of Non-Empty Tiles
	// Local Tile Start
	// Local Tile End
	// Legacy Tile Count
	// Bad Header

	wxPanel * numberOfTilesPropertyPanel = new wxPanel(m_artPropertiesPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	wxStaticText * numberOfTilesLabel = new wxStaticText(numberOfTilesPropertyPanel, wxID_ANY, "Number of Colour Tables:", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
	numberOfTilesLabel->SetFont(numberOfTilesLabel->GetFont().MakeBold());
	m_numberOfTilesText = new wxStaticText(numberOfTilesPropertyPanel, wxID_ANY, "0", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);

	m_tilesScrolledWindow = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_tilesScrolledWindow->SetScrollRate(0, 20);
	m_tilesScrolledWindow->SetBackgroundColour(GetBackgroundColour());

	std::shared_ptr<ColourTable> colourTable;
	std::vector<std::shared_ptr<Tile>> tiles(getArt()->getTiles());
	m_tilesScrolledWindow->Freeze();

	for(std::shared_ptr<Tile> & tile: tiles) {
		colourTable = PaletteDAT::DEFAULT_PALETTE_DAT->getColourTable(0);

		std::optional<uint8_t> optionalLookupColourTableIndex(tile->getLookupColourTableIndex());

		if(optionalLookupColourTableIndex.has_value()) {
			colourTable = PaletteDAT::DEFAULT_LOOKUP_DAT->getColourTable(optionalLookupColourTableIndex.value());
		}

		m_tilePanels.push_back(new TilePanel(tile, colourTable, m_tilesScrolledWindow));
	}

	m_tilesScrolledWindow->Thaw();

	const int border = 5;

	wxFlexGridSizer * numberOfTilesPropertySizer = new wxFlexGridSizer(2, 3, 3);
	numberOfTilesPropertySizer->Add(numberOfTilesLabel, 0, wxALIGN_CENTER_VERTICAL, border);
	numberOfTilesPropertySizer->Add(m_numberOfTilesText, 0, wxALIGN_CENTER_VERTICAL, border);
	numberOfTilesPropertyPanel->SetSizer(numberOfTilesPropertySizer);

	m_artPropertiesSizer = new wxWrapSizer(wxHORIZONTAL);
	m_artPropertiesSizer->Add(numberOfTilesPropertyPanel, 0, wxLEFT | wxRIGHT, border);
	m_artPropertiesPanel->SetSizer(m_artPropertiesSizer);

	m_artInfoSizer = new wxGridBagSizer(border, border);
	m_artInfoSizer->Add(m_artPropertiesPanel, wxGBPosition(0, 0), wxGBSpan(1, 1), wxEXPAND | wxHORIZONTAL, border);
	m_artInfoSizer->Add(m_tilesScrolledWindow, wxGBPosition(1, 0), wxGBSpan(1, 1), wxEXPAND | wxHORIZONTAL | wxVERTICAL, border);
	m_artInfoSizer->AddGrowableRow(1, 1);
	m_artInfoSizer->AddGrowableCol(0, 1);

	m_tilesScrolledWindowSizer = new wxWrapSizer(wxHORIZONTAL);
	for(TilePanel * tilePanel : m_tilePanels) {
		m_tilesScrolledWindowSizer->Add(tilePanel, 0, wxALL, border);
	}
	m_tilesScrolledWindow->SetSizer(m_tilesScrolledWindowSizer);

	SetSizer(m_artInfoSizer);

	update();
}

ArtPanel::~ArtPanel() { }

std::string ArtPanel::getPanelName() const {
	std::string_view fileName(getGameFile()->getFileName());

	return fileName.empty() ? fmt::format("NEW {} ART *", getGameFile()->getDefaultFileFormatExtension()) : fmt::format("{}{}", fileName, getGameFile()->isModified() ? " *" : "");
}

std::shared_ptr<const Art> ArtPanel::getArt() const {
	return std::dynamic_pointer_cast<const Art>(getGameFile());
}

std::shared_ptr<Art> ArtPanel::getArt() {
	return std::dynamic_pointer_cast<Art>(getGameFile());
}

void ArtPanel::update() {
	wxASSERT(wxIsMainThread());

	std::shared_ptr<const Art> art(getArt());

	if(art == nullptr) {
		return;
	}

	m_numberOfTilesText->SetLabel(std::to_string(art->numberOfTiles()));

	m_artPropertiesSizer->Layout();
}
