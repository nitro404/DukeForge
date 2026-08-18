#include "GroupPanelSSI.h"

#include <wx/gbsizer.h>

GroupPanelSSI::GroupPanelSSI(std::unique_ptr<GroupSSI> group, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style)
	: GroupPanel(std::move(group), parent, windowID, position, size, style)
	, m_ssiMetadataBox(nullptr)
	, m_ssiMetadataPanel(nullptr)
{
	const int border = 5;

	std::shared_ptr<GroupSSI> groupSSI(std::dynamic_pointer_cast<GroupSSI>(getGroup()));

	m_ssiMetadataBox = new wxStaticBox(this, wxID_ANY, "Sunstorm Interactive File Metadata", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	m_ssiMetadataBox->SetOwnFont(m_ssiMetadataBox->GetFont().MakeBold());

	m_ssiMetadataPanel = new SunstormInteractiveMetadataPanel(groupSSI, m_ssiMetadataBox);

	wxBoxSizer * ssiMetadataBoxSizer = new wxBoxSizer(wxHORIZONTAL);
	ssiMetadataBoxSizer->Add(m_ssiMetadataPanel, 1, wxEXPAND | wxALL, 20);
	m_ssiMetadataBox->SetSizer(ssiMetadataBoxSizer);

	m_groupInfoSizer->Add(m_ssiMetadataBox, wxGBPosition(2, 0), wxGBSpan(1, 2), wxEXPAND | wxHORIZONTAL, border);
	m_groupInfoSizer->AddGrowableRow(2, 0);
}

GroupPanelSSI::~GroupPanelSSI() { }
