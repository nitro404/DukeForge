#include "../IconUtilities.h"

void IconUtilities::setIcon(wxTopLevelWindow * window) {
	window->SetIcon(wxICON(DUKEFORGE_ICON));
}
