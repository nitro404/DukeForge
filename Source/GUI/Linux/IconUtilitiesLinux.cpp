#include "../IconUtilities.h"

#include "Linux/IconData.h"

void IconUtilities::setIcon(wxTopLevelWindow * window) {
	window->SetIcon(getIcon());
}
