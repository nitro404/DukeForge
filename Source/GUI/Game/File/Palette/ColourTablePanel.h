#ifndef _COLOUR_TABLE_PANEL_H_
#define _COLOUR_TABLE_PANEL_H_

#include "Game/File/Palette/ColourTable.h"

#include <boost/signals2.hpp>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <memory>

class ColourTablePanel final : public wxPanel {
public:
	ColourTablePanel(std::shared_ptr<ColourTable> colourTable, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~ColourTablePanel() override;

private:
	size_t getVerticalOffset() const;
	void updateColourTableNameLabel();
	void onColourTableModified(const ColourTable & colourTable);
	void onPaint(wxPaintEvent & event);
	void onMouseDown(wxMouseEvent & event);

	wxStaticText * m_colourTableNameLabel;
	std::shared_ptr<ColourTable> m_colourTable;
	boost::signals2::connection m_colourTableModifiedConnection;

	ColourTablePanel(const ColourTablePanel &) = delete;
	const ColourTablePanel & operator = (const ColourTablePanel &) = delete;
};

#endif // _COLOUR_TABLE_PANEL_H_
