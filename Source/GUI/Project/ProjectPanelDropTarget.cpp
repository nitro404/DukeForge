#include "ProjectPanel.h"

#include "GUI/WXUtilities.h"

ProjectPanel::DropTarget::DropTarget()
	: wxFileDropTarget() { }

ProjectPanel::DropTarget::~DropTarget() { }

bool ProjectPanel::DropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString & filePaths) {
	filesDropped(WXUtilities::createItemStringList(filePaths));

	return true;
}
