#ifndef _TILE_EDIT_DIALOG_H_
#define _TILE_EDIT_DIALOG_H_

#include "TileEditPanel.h"

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <boost/signals2.hpp>

#include <memory>
#include <string>

class TileEditDialog final : public wxDialog {
public:
	TileEditDialog(std::shared_ptr<Tile> tile, std::shared_ptr<ColourTable> colourTable, wxWindow * parent);
	~TileEditDialog() override;

	void close();

private:
	void onCloseButtonPressed(wxCommandEvent & event);
	void onClose(wxCloseEvent & closeEvent);

	TileEditDialog(const TileEditDialog &) = delete;
	const TileEditDialog & operator = (const TileEditDialog &) = delete;

	wxDECLARE_EVENT_TABLE();
};

#endif // _TILE_EDIT_DIALOG_H_
