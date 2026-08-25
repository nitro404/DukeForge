#include "TileEditDialog.h"

#include <fmt/core.h>

TileEditDialog::TileEditDialog(std::shared_ptr<Tile> tile, std::shared_ptr<ColourTable> colourTable, wxWindow * parent)
// TODO: leading zeroes, names.h:
	: wxDialog(parent, wxID_ANY, tile != nullptr ? fmt::format("TILE{}", tile->getNumber()) : "", wxDefaultPosition, wxDefaultSize, wxCAPTION | wxCLOSE_BOX, "Tile Edit Dialog") {
	wxASSERT(wxIsMainThread());

#if defined(WINDOWS)
	SetIcon(wxICON(DUKEFORGE_ICON));
#endif // WINDOWS

	Freeze();

	const int border = 5;

	TileEditPanel * tileEditPanel = new TileEditPanel(tile, colourTable, this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER);

	wxGridSizer * tileEditDialogSizer = new wxGridSizer(1, 1, border);
	tileEditDialogSizer->Add(tileEditPanel, 1, wxEXPAND | wxALL, border);
	SetSizer(tileEditDialogSizer);
	Fit();

	Thaw();
}

TileEditDialog::~TileEditDialog() { }

void TileEditDialog::close() {
	wxASSERT(wxIsMainThread());

	// TODO
}

void TileEditDialog::onClose(wxCloseEvent & closeEvent) {
	close();
}

wxBEGIN_EVENT_TABLE(TileEditDialog, wxDialog)
	EVT_CLOSE(TileEditDialog::onClose)
wxEND_EVENT_TABLE()
