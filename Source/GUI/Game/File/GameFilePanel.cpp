#include "GameFilePanel.h"

#include "GUI/WXUtilities.h"
#include "Game/File/GameFile.h"

#include <Utilities/FileUtilities.h>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

GameFilePanel::GameFilePanel(std::shared_ptr<GameFile> gameFile, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style, const std::string & name)
	: wxPanel(parent, windowID, position, size, style, name)
	, m_gameFile(gameFile) {
	wxASSERT(wxIsMainThread());

	m_gameFileModifiedConnection = m_gameFile->modified.connect(std::bind(&GameFilePanel::onGameFileModified, this, std::placeholders::_1));
}

GameFilePanel::~GameFilePanel() {
	m_gameFileModifiedConnection.disconnect();
}

std::shared_ptr<const GameFile> GameFilePanel::getGameFile() const {
	return m_gameFile;
}

std::shared_ptr<GameFile> GameFilePanel::getGameFile() {
	return m_gameFile;
}

bool GameFilePanel::isModified() const {
	return m_gameFile->isModified();
}

void GameFilePanel::onGameFileModified(const GameFile & gameFile) {
	update();

	gameFileModified(*this);
}
