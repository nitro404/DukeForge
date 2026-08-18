#include "GroupPanelGRP.h"

GroupPanelGRP::GroupPanelGRP(std::unique_ptr<GroupGRP> group, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style)
	: GroupPanel(std::move(group), parent, windowID, position, size, style)
{
}

GroupPanelGRP::~GroupPanelGRP() { }
