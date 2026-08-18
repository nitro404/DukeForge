#ifndef _GROUP_PANEL_SSI_H_
#define _GROUP_PANEL_SSI_H_

#include "Game/File/Group/SSI/GroupSSI.h"
#include "GUI/Game/File/Group/GroupPanel.h"
#include "GUI/Game/File/Group/SSI/SunstormInteractiveMetadataPanel.h"

class GroupPanelSSI final : public GroupPanel {
public:
	GroupPanelSSI(std::unique_ptr<GroupSSI> group, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~GroupPanelSSI() override;

private:
	wxStaticBox * m_ssiMetadataBox;
	SunstormInteractiveMetadataPanel * m_ssiMetadataPanel;

	GroupPanelSSI(const GroupPanelSSI &) = delete;
	const GroupPanelSSI & operator = (const GroupPanelSSI &) = delete;
};

#endif // _GROUP_PANEL_SSI_H_
