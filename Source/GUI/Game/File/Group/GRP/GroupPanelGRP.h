#ifndef _GROUP_PANEL_GRP_H_
#define _GROUP_PANEL_GRP_H_

#include "Game/File/Group/GRP/GroupGRP.h"
#include "GUI/Game/File/Group/GroupPanel.h"

class GroupPanelGRP final : public GroupPanel {
public:
	GroupPanelGRP(std::unique_ptr<GroupGRP> group, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~GroupPanelGRP() override;

private:
	GroupPanelGRP(const GroupPanelGRP &) = delete;
	const GroupPanelGRP & operator = (const GroupPanelGRP &) = delete;
};

#endif // _GROUP_PANEL_GRP_H_
