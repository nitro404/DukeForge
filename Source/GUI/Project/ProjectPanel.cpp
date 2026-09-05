#include "ProjectPanel.h"

#include "Application/SettingsManager.h"
#include "Game/File/GameFile.h"
#include "Game/File/Group/Group.h"
#include "Game/File/Group/GroupFile.h"
#include "Game/File/GameFileFactoryRegistry.h"
#include "GUI/Game/File/GameFilePanel.h"
#include "GUI/Game/File/GameFilePanelFactoryRegistry.h"
#include "GUI/Game/File/Group/GroupPanel.h"
#include "GUI/WXUtilities.h"

#include <Signal/SignalConnectionGroup.h>
#include <Utilities/FileUtilities.h>

#include <spdlog/spdlog.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/gbsizer.h>
#include <wx/textdlg.h>
#include <wx/wrapsizer.h>

#include <any>
#include <map>
#include <filesystem>
#include <sstream>

ProjectPanel::ProjectPanel(wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style)
	: wxPanel(parent, windowID, position, size, style, "Project")
	, m_notebook(nullptr) {
	wxASSERT(wxIsMainThread());

	Freeze();

	SetDropTarget(this);
	SetBackgroundStyle(wxBG_STYLE_PAINT);

	m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP | wxNB_MULTILINE, "Game Files");
	m_notebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &ProjectPanel::onNotebookPageChanged, this);

	int border = 2;

	wxGridBagSizer * ProjectPanelSizer = new wxGridBagSizer(border, border);
	ProjectPanelSizer->Add(m_notebook, wxGBPosition(0, 0), wxGBSpan(1, 1), wxEXPAND | wxALL, border);
	ProjectPanelSizer->AddGrowableRow(0, 1);
	ProjectPanelSizer->AddGrowableCol(0, 1);
	SetSizerAndFit(ProjectPanelSizer);

	updateGameFilePanelNames();

	Thaw();
}

ProjectPanel::~ProjectPanel() { }

std::string ProjectPanel::getPanelName() const {
	return "New Project";
}

bool ProjectPanel::hasGameFilePanel(const GameFilePanel * gameFilePanel) const {
	wxASSERT(wxIsMainThread());

	if(gameFilePanel == nullptr) {
		return false;
	}

	for(size_t i = 0; i < m_notebook->GetPageCount(); i++) {
		if(m_notebook->GetPage(i) == gameFilePanel) {
			return true;
		}
	}

	return false;
}

bool ProjectPanel::hasPanelWithGameFile(const GameFile * gameFile) const {
	wxASSERT(wxIsMainThread());

	if(gameFile == nullptr) {
		return false;
	}

	for(size_t i = 0; i < m_notebook->GetPageCount(); i++) {
		if(getGameFile(i).get() == gameFile) {
			return true;
		}
	}

	return false;
}

bool ProjectPanel::hasPanelWithGameFilePath(const std::string & filePath) const {
	wxASSERT(wxIsMainThread());

	if(filePath.empty()) {
		return false;
	}

	for(size_t i = 0; i < m_notebook->GetPageCount(); i++) {
		if(Utilities::areStringsEqual(getGameFile(i)->getFilePath(), filePath)) {
			return true;
		}
	}

	return false;
}

size_t ProjectPanel::indexOfGameFilePanel(const GameFilePanel * gameFilePanel) const {
	wxASSERT(wxIsMainThread());

	if(gameFilePanel == nullptr) {
		return std::numeric_limits<size_t>::max();
	}

	for(size_t i = 0; i < m_notebook->GetPageCount(); i++) {
		if(m_notebook->GetPage(i) == gameFilePanel) {
			return i;
		}
	}

	return std::numeric_limits<size_t>::max();
}

size_t ProjectPanel::indexOfPanelWithGameFile(const GameFile * gameFile) const {
	wxASSERT(wxIsMainThread());

	if(gameFile == nullptr) {
		return std::numeric_limits<size_t>::max();
	}

	for(size_t i = 0; i < m_notebook->GetPageCount(); i++) {
		if(getGameFile(i).get() == gameFile) {
			return i;
		}
	}

	return std::numeric_limits<size_t>::max();
}

size_t ProjectPanel::indexOfPanelWithGameFileFilePath(const std::string & filePath) const {
	wxASSERT(wxIsMainThread());

	std::error_code errorCode;
	std::filesystem::path actualFilePath(filePath);

	for(size_t i = 0; i < m_notebook->GetPageCount(); i++) {
		if(std::filesystem::equivalent(std::filesystem::path(getGameFile(i)->getFilePath()), actualFilePath, errorCode)) {
			return i;
		}
	}

	return std::numeric_limits<size_t>::max();
}

size_t ProjectPanel::numberOfGameFilePanels() const {
	wxASSERT(wxIsMainThread());

	return m_notebook->GetPageCount();
}

GameFilePanel * ProjectPanel::getGameFilePanel(size_t gameFilePanelIndex) const {
	wxASSERT(wxIsMainThread());

	if(gameFilePanelIndex >= m_notebook->GetPageCount()) {
		return nullptr;
	}

	return dynamic_cast<GameFilePanel *>(m_notebook->GetPage(gameFilePanelIndex));
}

GameFilePanel * ProjectPanel::getPanelWithGameFile(const GameFile * gameFile) const {
	return getGameFilePanel(indexOfPanelWithGameFile(gameFile));
}

GameFilePanel * ProjectPanel::getPanelWithGameFileFilePath(const std::string & filePath) const {
	return getGameFilePanel(indexOfPanelWithGameFileFilePath(filePath));
}

GameFilePanel * ProjectPanel::getCurrentGameFilePanel() const {
	wxASSERT(wxIsMainThread());

	return dynamic_cast<GameFilePanel *>(m_notebook->GetCurrentPage());
}

std::shared_ptr<GameFile> ProjectPanel::getGameFile(size_t gameFilePanelIndex) const {
	GameFilePanel * gameFilePanel = getGameFilePanel(gameFilePanelIndex);

	if(gameFilePanel == nullptr) {
		return nullptr;
	}

	return gameFilePanel->getGameFile();
}

std::shared_ptr<GameFile> ProjectPanel::getCurrentGameFile() const {
	GameFilePanel * gameFilePanel = getCurrentGameFilePanel();

	if(gameFilePanel == nullptr) {
		return nullptr;
	}

	return gameFilePanel->getGameFile();
}

bool ProjectPanel::hasUnsavedGameFiles() const {
	for(size_t i = 0; i < numberOfGameFilePanels(); i++) {
		GameFilePanel * gameFilePanel = getGameFilePanel(i);

		if(gameFilePanel->isModified()) {
			return true;
		}
	}

	return false;
}

bool ProjectPanel::updateGameFilePanel(size_t gameFilePanelIndex) {
	GameFilePanel * gameFilePanel = getGameFilePanel(gameFilePanelIndex);

	if(gameFilePanel == nullptr) {
		return false;
	}

	updateGameFilePanelName(gameFilePanelIndex);

	return true;
}

void ProjectPanel::updateGameFilePanelNames() {
	wxASSERT(wxIsMainThread());

	for(size_t i = 0; i < m_notebook->GetPageCount(); i++) {
		updateGameFilePanelName(i);
	}
}

bool ProjectPanel::updateGameFilePanelName(size_t gameFilePanelIndex) {
	wxASSERT(wxIsMainThread());

	GameFilePanel * gameFilePanel = getGameFilePanel(gameFilePanelIndex);

	if(gameFilePanel == nullptr) {
		return false;
	}

	m_notebook->SetPageText(gameFilePanelIndex, gameFilePanel->getPanelName());
	m_notebook->Refresh();

	return true;
}

void ProjectPanel::addGameFilePanel(GameFilePanel * gameFilePanel) {
	wxASSERT(wxIsMainThread());

	if(gameFilePanel == nullptr) {
		return;
	}

	SignalConnectionGroup signalConnections(
		gameFilePanel->gameFileModified.connect(std::bind(&ProjectPanel::onGameFileModified, this, std::placeholders::_1))
	);

	if(dynamic_cast<const GroupPanel *>(gameFilePanel) != nullptr) {
		static_cast<GroupPanel *>(gameFilePanel)->groupFileSelectionChanged.connect(std::bind(&ProjectPanel::onGroupFileSelectionChanged, this, std::placeholders::_1));
	}

	m_gameFilePanelConnections.push_back(std::move(signalConnections));

	const std::string gameFilePanelName(gameFilePanel->getPanelName());

	m_notebook->AddPage(gameFilePanel, gameFilePanelName);
	m_notebook->ChangeSelection(m_notebook->GetPageCount() - 1);

	activeGameFilePanelChanged(*this, getCurrentGameFilePanel());
}

bool ProjectPanel::newGameFile() {
	wxASSERT(wxIsMainThread());

	GameFileFactoryRegistry * gameFileFactoryRegistry = GameFileFactoryRegistry::getInstance();
	GameFilePanelFactoryRegistry * gameFilePanelFactoryRegistry = GameFilePanelFactoryRegistry::getInstance();

	if(gameFileFactoryRegistry == nullptr) {
		WXUtilities::showErrorMessage("Missing the game file factory registry", "Game File Factory Registry Missing", this);
		return false;
	}

	if(gameFilePanelFactoryRegistry == nullptr) {
		WXUtilities::showErrorMessage("Missing the game file panel factory registry", "Game File Panel Factory Registry Missing", this);
		return false;
	}

	std::vector<std::pair<std::string, std::string>> allFileFormatNamesAndExtensions(gameFilePanelFactoryRegistry->getAllFileFormatNamesAndExtensions());
	std::vector<std::string> allFileFormatNames;
	std::ranges::transform(allFileFormatNamesAndExtensions, std::back_inserter(allFileFormatNames), [](const std::pair<std::string, std::string> & filleFormatNameAndExtension) { return filleFormatNameAndExtension.first; });

	int selectedGameFileTypeIndex = wxGetSingleChoiceIndex(
		"Please choose a game file type to create:",
		"Choose Game File Type",
		WXUtilities::createItemWXArrayString(allFileFormatNames),
		0,
		this
	);

	if(selectedGameFileTypeIndex == wxNOT_FOUND || selectedGameFileTypeIndex >= allFileFormatNamesAndExtensions.size()) {
		return false;
	}

	const std::string & selectedGameFileFormatName = allFileFormatNamesAndExtensions[selectedGameFileTypeIndex].second;
	const std::string & selectedGameFileFormatFileExtension = allFileFormatNamesAndExtensions[selectedGameFileTypeIndex].second;

	std::unique_ptr<GameFile> newGameFile(gameFileFactoryRegistry->createNewGameFile(selectedGameFileFormatFileExtension));

	if(newGameFile == nullptr) {
		WXUtilities::showErrorMessage(fmt::format("Failed to create new '{}' game file.", selectedGameFileFormatName), "Game File Creation Failed", this);
		return false;
	}

	GameFilePanel * newGameFilePanel = gameFilePanelFactoryRegistry->createNewGameFilePanel(std::move(newGameFile), m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	if(newGameFilePanel == nullptr) {
		WXUtilities::showErrorMessage(fmt::format("Failed to create new '{}' game file panel.", selectedGameFileFormatName), "Game File Panel Creation Failed", this);
		return false;
	}

	addGameFilePanel(newGameFilePanel);

	return true;
}

bool ProjectPanel::openGameFile(const std::string & filePath) {
	wxASSERT(wxIsMainThread());

	GameFileFactoryRegistry * gameFileFactoryRegistry = GameFileFactoryRegistry::getInstance();
	GameFilePanelFactoryRegistry * gameFilePanelFactoryRegistry = GameFilePanelFactoryRegistry::getInstance();

	if(gameFileFactoryRegistry == nullptr) {
		WXUtilities::showErrorMessage("Missing the game file factory registry", "Game File Factory Registry Missing", this);
		return false;
	}

	if(gameFilePanelFactoryRegistry == nullptr) {
		WXUtilities::showErrorMessage("Missing the game file panel factory registry", "Game File Panel Factory Registry Missing", this);
		return false;
	}

	size_t existingGameFilePanelIndex = indexOfPanelWithGameFileFilePath(filePath);

	if(existingGameFilePanelIndex != std::numeric_limits<size_t>::max()) {
		WXUtilities::showInfoMessage(fmt::format("Game file '{}' already open!", Utilities::getFileName(filePath)), "Game File Already Open", this);

		m_notebook->ChangeSelection(existingGameFilePanelIndex);

		return false;
	}

	std::unique_ptr<GameFile> gameFile(gameFileFactoryRegistry->loadGameFileFrom(filePath));

	if(gameFile == nullptr) {
		WXUtilities::showErrorMessage(fmt::format("Failed to load game file from: '{}'.", filePath), "Game File Load Failed", this);
		return false;
	}

	if(!gameFile->isValid()) {
		WXUtilities::showErrorMessage(fmt::format("Invalid game file: '{}'.", filePath), "Invalid Game File", this);
		return false;
	}

	if(hasPanelWithGameFilePath(gameFile->getFilePath())) {
		return false;
	}

	GameFilePanel * gameFilePanel = gameFilePanelFactoryRegistry->createNewGameFilePanel(std::move(gameFile), m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	if(gameFilePanel == nullptr) {
		WXUtilities::showErrorMessage(fmt::format("Failed to create new '{}' game file panel for file: '{}'.", gameFile->getFileFormatName(), filePath), "Game File Panel Creation Failed", this);
		return false;
	}

	addGameFilePanel(gameFilePanel);

	return true;
}

size_t ProjectPanel::openGameFiles(const std::vector<std::string> & gameFilePaths) {
	wxASSERT(wxIsMainThread());

	size_t openedGameFileCount = 0;

	for(size_t i = 0; i < gameFilePaths.size(); i++) {
		if(openGameFile(gameFilePaths[i])) {
			openedGameFileCount++;
		}
	}

	return openedGameFileCount;
}

size_t ProjectPanel::openGameFiles() {
	wxASSERT(wxIsMainThread());

	GameFilePanelFactoryRegistry * gameFilePanelFactoryRegistry = GameFilePanelFactoryRegistry::getInstance();

	if(gameFilePanelFactoryRegistry == nullptr) {
		WXUtilities::showErrorMessage("Missing the game file panel factory registry", "Game File Panel Factory Registry Missing", this);
		return false;
	}

	wxFileDialog openFilesDialog(this, "Open Game File(s)", std::filesystem::current_path().string(), "", gameFilePanelFactoryRegistry->getAllFileDialogFileTypesAsSeparateStrings(), wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST, wxDefaultPosition, wxDefaultSize, "Open Game Files");
	int openFilesResult = openFilesDialog.ShowModal();

	if(openFilesResult == wxID_CANCEL) {
		return 0;
	}

	size_t openedGameFileCount = 0;
	wxArrayString filePathArray;
	openFilesDialog.GetPaths(filePathArray);

	for(size_t i = 0; i < filePathArray.GetCount(); i++) {
		if(openGameFile(filePathArray[i].ToStdString())) {
			openedGameFileCount++;
		}
	}

	return openedGameFileCount;
}

bool ProjectPanel::createGroupFromDirectory(const std::string & directoryPath) {
	wxASSERT(wxIsMainThread());

	GameFileFactoryRegistry * gameFileFactoryRegistry = GameFileFactoryRegistry::getInstance();
	GameFilePanelFactoryRegistry * gameFilePanelFactoryRegistry = GameFilePanelFactoryRegistry::getInstance();

	if(gameFileFactoryRegistry == nullptr) {
		WXUtilities::showErrorMessage("Missing the game file factory registry", "Game File Factory Registry Missing", this);
		return false;
	}

	if(gameFilePanelFactoryRegistry == nullptr) {
		WXUtilities::showErrorMessage("Missing the game file panel factory registry", "Game File Panel Factory Registry Missing", this);
		return false;
	}

	std::vector<std::pair<std::string, std::string>> allFileFormatNamesAndExtensions(gameFilePanelFactoryRegistry->getAllFileFormatNamesAndExtensions());
	std::vector<std::string> allFileFormatNames;
	std::ranges::transform(allFileFormatNamesAndExtensions, std::back_inserter(allFileFormatNames), [](const std::pair<std::string, std::string> & filleFormatNameAndExtension) { return filleFormatNameAndExtension.first; });

	int selectedGroupTypeIndex = wxGetSingleChoiceIndex(
		"Please choose a group type to create:",
		"Choose Group Type",
		WXUtilities::createItemWXArrayString(allFileFormatNames),
		0,
		this
	);

	if(selectedGroupTypeIndex == wxNOT_FOUND || selectedGroupTypeIndex >= allFileFormatNamesAndExtensions.size()) {
		return false;
	}

	const std::string & selectedGameFileFormatFileExtension = allFileFormatNamesAndExtensions[selectedGroupTypeIndex].second;

	std::unique_ptr<GameFile> newGameFile(gameFileFactoryRegistry->createNewGameFile(selectedGameFileFormatFileExtension));

	if(dynamic_cast<const Group *>(newGameFile.get()) == nullptr) {
		WXUtilities::showErrorMessage("Game file is not a group.", "Invalid Game File Type", this);
		return false;
	}

	std::unique_ptr<Group> newGroup(static_cast<Group *>(newGameFile.release()));
	newGroup->addFiles(Group::createGroupFilesFromDirectory(directoryPath));

	if(!Group::isValid(newGroup.get())) {
		WXUtilities::showErrorMessage(fmt::format("Failed to create group from directory: '{}'.", directoryPath), "Group Creation Failed", this);
		return false;
	}

	GroupPanel * groupPanel = new GroupPanel(std::move(newGroup), m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	std::shared_ptr<const Group> sharedGroup(groupPanel->getGroup());

	addGameFilePanel(groupPanel);

	return true;
}

bool ProjectPanel::createGroupFromDirectory() {
	wxASSERT(wxIsMainThread());

	wxDirDialog selectDirectoryDialog(this, "Create Group from Directory", std::filesystem::current_path().string(), wxDD_DIR_MUST_EXIST, wxDefaultPosition, wxDefaultSize, "Create Group");
	int selectDirectoryResult = selectDirectoryDialog.ShowModal();

	if(selectDirectoryResult == wxID_CANCEL) {
		return false;
	}

	return createGroupFromDirectory(selectDirectoryDialog.GetPath().ToStdString());
}

bool ProjectPanel::saveGameFile(GameFile * gameFile) {
	wxASSERT(wxIsMainThread());

	GameFilePanelFactoryRegistry * gameFilePanelFactoryRegistry = GameFilePanelFactoryRegistry::getInstance();

	if(gameFilePanelFactoryRegistry == nullptr) {
		WXUtilities::showErrorMessage("Missing the game file panel factory registry", "Game File Panel Factory Registry Missing", this);
		return false;
	}

	if(!GameFile::isValid(gameFile)) {
		WXUtilities::showErrorMessage("Invalid game file.", "Invalid Game File", this);
		return false;
	}

	if(gameFile->getFilePath().empty()) {
		wxFileDialog saveFileDialog(this, "Save Game File", std::filesystem::current_path().string(), "NEW." + gameFile->getDefaultFileFormatExtension(), gameFilePanelFactoryRegistry->getFileDialogFileType(gameFile->getDefaultFileFormatExtension()), wxFD_SAVE, wxDefaultPosition, wxDefaultSize, "Save Game File");
		int saveFileResult = saveFileDialog.ShowModal();

		if(saveFileResult == wxID_CANCEL) {
			return false;
		}

		std::string newFilePath(saveFileDialog.GetPath());

		if(std::filesystem::exists(std::filesystem::path(newFilePath))) {
			int overwriteFileConfirmationResult = wxMessageBox(fmt::format("Game file already exists at path:\n\n{}\n\n Are you sure that want to overwrite this file?", newFilePath), "Overwrite Game File", wxYES_NO | wxCANCEL | wxICON_WARNING, this);

			if(overwriteFileConfirmationResult != wxYES) {
				return false;
			}
		}

		gameFile->setFilePath(newFilePath);

		updateGameFilePanelName(indexOfPanelWithGameFile(gameFile));
	}

	if(gameFile->save()) {
		spdlog::info("Successfully saved game file: '{}'.", gameFile->getFilePath());
	}
	else {
		WXUtilities::showErrorMessage(fmt::format("Failed to save game file: '{}'.", gameFile->getFilePath()), "Game File Writing Failed", this);
		return false;
	}

	return true;
}

bool ProjectPanel::saveCurrentGameFile() {
	wxASSERT(wxIsMainThread());

	return saveGameFile(getCurrentGameFile().get());
}

bool ProjectPanel::saveGameFileAs(GameFile * gameFile) {
	wxASSERT(wxIsMainThread());

	GameFilePanelFactoryRegistry * gameFilePanelFactoryRegistry = GameFilePanelFactoryRegistry::getInstance();

	if(gameFilePanelFactoryRegistry == nullptr) {
		WXUtilities::showErrorMessage("Missing the game file panel factory registry", "Game File Panel Factory Registry Missing", this);
		return false;
	}

	if(!GameFile::isValid(gameFile)) {
		WXUtilities::showErrorMessage("Invalid game file.", "Invalid Game File", this);
		return false;
	}

	std::string basePath(Utilities::getBasePath(gameFile->getFilePath()));

	if(basePath.empty()) {
		basePath = std::filesystem::current_path().string();
	}

	std::string previousFileName(gameFile->getFileName());
	std::string newFileName;

	if(!gameFile->getFilePath().empty()) {
		newFileName = gameFile->getFileName();
	}

	wxFileDialog saveFileAsDialog(this, "Save Game File to New File", Utilities::getAbsoluteFilePath(basePath, std::filesystem::current_path().string()), newFileName.empty() ? "NEW." + gameFile->getDefaultFileFormatExtension() : newFileName, gameFilePanelFactoryRegistry->getFileDialogFileType(gameFile->getDefaultFileFormatExtension()), wxFD_SAVE, wxDefaultPosition, wxDefaultSize, "Save Game File As");
	int saveFileAsResult = saveFileAsDialog.ShowModal();

	if(saveFileAsResult == wxID_CANCEL) {
		return false;
	}

	std::string newFilePath(saveFileAsDialog.GetPath());

	if(std::filesystem::exists(std::filesystem::path(newFilePath))) {
		int overwriteFileConfirmationResult = wxMessageBox(fmt::format("Game file already exists at path:\n\n{}\n\n Are you sure that want to overwrite this file?", newFilePath), "Overwrite Game File", wxYES_NO | wxCANCEL | wxICON_WARNING, this);

		if(overwriteFileConfirmationResult != wxYES) {
			return false;
		}
	}

	gameFile->setFilePath(newFilePath);

	updateGameFilePanelName(indexOfPanelWithGameFile(gameFile));

	if(gameFile->save()) {
		spdlog::info("Successfully saved game file to new file: '{}'.", gameFile->getFilePath());
	}
	else {
		WXUtilities::showErrorMessage(fmt::format("Failed to save game file to new file: '{}'.", gameFile->getFilePath()), "Game File Writing Failed", this);
		return false;
	}

	return true;
}

bool ProjectPanel::saveCurrentGameFileAs() {
	wxASSERT(wxIsMainThread());

	return saveGameFileAs(getCurrentGameFile().get());
}

bool ProjectPanel::saveAllGameFiles() {
	wxASSERT(wxIsMainThread());

	bool allGameFilesSaved = true;
	GameFilePanel * gameFilePanel = nullptr;

	for(size_t i = 0; i < numberOfGameFilePanels(); i++) {
		gameFilePanel = getGameFilePanel(i);

		if(gameFilePanel == nullptr) {
			continue;
		}

		if(!saveGameFile(gameFilePanel->getGameFile().get())) {
			allGameFilesSaved = false;
		}
	}

	return allGameFilesSaved;
}

size_t ProjectPanel::addFilesToGroup(Group * group) {
	wxASSERT(wxIsMainThread());

	if(!Group::isValid(group)) {
		WXUtilities::showErrorMessage("Invalid group file.", "Invalid Group File", this);
		return 0;
	}

	wxFileDialog addFilesDialog(this, "Add Files to Group", std::filesystem::current_path().string(), "", WXUtilities::FILE_DIALOG_ALL_FILES, wxFD_OPEN | wxFD_MULTIPLE | wxFD_FILE_MUST_EXIST, wxDefaultPosition, wxDefaultSize, "Add Files");
	int addFilesResult = addFilesDialog.ShowModal();

	if(addFilesResult == wxID_CANCEL) {
		return 0;
	}

	bool replaceExistingFiles = false;
	std::string formattedFileName;
	std::vector<std::string> filePaths;
	std::vector<std::string> existingFileNames;
	std::stringstream existingFileNamesStream;
	wxArrayString filePathArray;
	addFilesDialog.GetPaths(filePathArray);

	for(size_t i = 0; i < filePathArray.GetCount(); i++) {
		filePaths.push_back(filePathArray[i].ToStdString());

		formattedFileName = GroupFile::formatFileName(Utilities::getFileName(filePaths.back()));

		if(group->hasFileWithName(formattedFileName)) {
			bool duplicateFileName = false;

			for(const std::string & currentExistingFileName : existingFileNames) {
				if(Utilities::areStringsEqualIgnoreCase(currentExistingFileName, formattedFileName)) {
					duplicateFileName = true;
					break;
				}
			}

			if(!duplicateFileName) {
				existingFileNames.push_back(formattedFileName);

				if(existingFileNamesStream.tellp() != 0) {
					existingFileNamesStream << ", ";
				}

				existingFileNamesStream << '\'' << formattedFileName << '\'';
			}
		}
	}

	if(!existingFileNames.empty()) {
		bool singleFile = existingFileNames.size() == 1;

		int replaceFilesResult = wxMessageBox(
			fmt::format(
				"File{0} with name{0} {1} already exist{2}! Would you like to replace {3}?",
				singleFile ? "" : "s",
				existingFileNamesStream.str(),
				singleFile ? "s" : "",
				singleFile ? "this file" : "these files"
			),
			fmt::format(
				"Replace File{}",
				singleFile ? "" : "s"
			),
			wxICON_QUESTION | wxYES_NO | wxCANCEL,
			this
		);

		if(replaceFilesResult == wxYES) {
			replaceExistingFiles = true;
		}
		else if(replaceFilesResult == wxNO) {
			replaceExistingFiles = false;
		}
		else if(replaceFilesResult == wxCANCEL) {
			return 0;
		}
	}

	return group->addFiles(filePaths, replaceExistingFiles);
}

size_t ProjectPanel::addFilesToCurrentGroup() {
	wxASSERT(wxIsMainThread());

	std::shared_ptr<Group> group(std::dynamic_pointer_cast<Group>(getCurrentGameFile()));

	if(group == nullptr) {
		return 0u;
	}

	return addFilesToGroup(group.get());
}

size_t ProjectPanel::removeSelectedFilesFromGroup(Group * group) {
	wxASSERT(wxIsMainThread());

	if(!Group::isValid(group)) {
		return 0;
	}

	GroupPanel * groupPanel = dynamic_cast<GroupPanel *>(getPanelWithGameFile(group));

	if(groupPanel == nullptr) {
		WXUtilities::showErrorMessage("Current game file panel does not contain a group.", "Incorrect Panel Type", this);
		return 0;
	}

	std::vector<std::shared_ptr<GroupFile>> selectedFiles(groupPanel->getSelectedFiles());

	return group->removeFiles(selectedFiles);
}

size_t ProjectPanel::removeSelectedFilesFromCurrentGroup() {
	wxASSERT(wxIsMainThread());

	std::shared_ptr<Group> group(std::dynamic_pointer_cast<Group>(getCurrentGameFile()));

	if(group == nullptr) {
		return 0u;
	}

	return removeSelectedFilesFromGroup(group.get());
}

bool ProjectPanel::replaceSelectedFileInGroup(Group * group) {
	wxASSERT(wxIsMainThread());

	if(!Group::isValid(group)) {
		WXUtilities::showErrorMessage("Invalid group file.", "Invalid Group File", this);
		return false;
	}

	GroupPanel * groupPanel = dynamic_cast<GroupPanel *>(getPanelWithGameFile(group));

	if(groupPanel == nullptr) {
		WXUtilities::showErrorMessage("Current game file panel does not contain a group.", "Incorrect Panel Type", this);
		return false;
	}

	std::vector<std::shared_ptr<GroupFile>> selectedFiles(groupPanel->getSelectedFiles());

	if(selectedFiles.size() != 1) {
		return false;
	}

	std::shared_ptr<GroupFile> selectedFile(selectedFiles[0]);

	wxFileDialog replaceFileDialog(this, "Select a Replacement File", std::filesystem::current_path().string(), "", "All Files (*.*)|*.*", wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition, wxDefaultSize, "Replace File");
	int replaceFileResult = replaceFileDialog.ShowModal();

	if(replaceFileResult == wxID_CANCEL) {
		return false;
	}

	bool renameFile = false;
	size_t indexOfExistingFileWithNewName = std::numeric_limits<size_t>::max();
	std::string newFilePath(replaceFileDialog.GetPath());
	std::string formattedNewFileName(GroupFile::formatFileName(Utilities::getFileName(newFilePath)));

	if(!Utilities::areStringsEqualIgnoreCase(selectedFile->getFileName(), formattedNewFileName)) {
		int renameExistingFileResult = wxMessageBox(fmt::format("Would you like to change the file name from '{}' to '{}', or keep the previous file name instead?", selectedFile->getFileName(), formattedNewFileName), "Rename File", wxICON_QUESTION | wxYES_NO | wxCANCEL, this);

		if(renameExistingFileResult == wxYES) {
			renameFile = true;
		}
		else if(renameExistingFileResult == wxNO) {
			renameFile = false;
		}
		else if(renameExistingFileResult == wxCANCEL) {
			return false;
		}

		if(renameFile) {
			indexOfExistingFileWithNewName = group->indexOfFileWithName(formattedNewFileName);

			if(indexOfExistingFileWithNewName != std::numeric_limits<size_t>::max()) {
				int replaceExistingFileResult = wxMessageBox(fmt::format("Group already has a file named '{}'! Would you like to replace it?", formattedNewFileName), "Replace File", wxICON_QUESTION | wxYES_NO | wxCANCEL, this);

				if(replaceExistingFileResult != wxYES) {
					return false;
				}
			}
		}
	}

	if(indexOfExistingFileWithNewName != std::numeric_limits<size_t>::max()) {
		if(!group->removeFile(indexOfExistingFileWithNewName)) {
			return false;
		}
	}

	size_t existingGroupFileIndex = group->indexOfFile(*selectedFile);
	std::string previousGroupFileName(selectedFile->getFileName());
	size_t previousGroupFileSize = selectedFile->getSize();

	if(!group->replaceFile(*selectedFile, newFilePath, !renameFile)) {
		return false;
	}

	return true;
}

bool ProjectPanel::replaceSelectedFileInCurrentGroup() {
	wxASSERT(wxIsMainThread());

	std::shared_ptr<Group> group(std::dynamic_pointer_cast<Group>(getCurrentGameFile()));

	if(group == nullptr) {
		WXUtilities::showErrorMessage("Current game file panel does not contain a group.", "Incorrect Panel Type", this);
		return false;
	}

	return replaceSelectedFileInGroup(group.get());
}

bool ProjectPanel::renameSelectedFileInGroup(Group * group) {
	wxASSERT(wxIsMainThread());

	if(!Group::isValid(group)) {
		WXUtilities::showErrorMessage("Invalid group file.", "Invalid Group File", this);
		return false;
	}

	GroupPanel * groupPanel = dynamic_cast<GroupPanel *>(getPanelWithGameFile(group));

	if(groupPanel == nullptr) {
		WXUtilities::showErrorMessage("Current game file panel does not contain a group.", "Incorrect Panel Type", this);
		return false;
	}

	std::vector<std::shared_ptr<GroupFile>> selectedFiles(groupPanel->getSelectedFiles());

	if(selectedFiles.size() != 1) {
		WXUtilities::showErrorMessage("Only one file can be selected.", "Invalid Selection", this);
		return false;
	}

	std::shared_ptr<GroupFile> selectedFile(selectedFiles[0]);

	wxTextEntryDialog newFileNameDialog(this, "Enter a new file name:", "Rename File", selectedFile->getFileName(), wxOK | wxCANCEL, wxDefaultPosition);
	newFileNameDialog.SetMaxLength(GroupFile::MAX_FILE_NAME_LENGTH);
	int renameFileResult = newFileNameDialog.ShowModal();

	if(renameFileResult == wxID_CANCEL) {
		return false;
	}

	std::string newFileName(newFileNameDialog.GetValue());

	if(newFileName.empty() || Utilities::areStringsEqualIgnoreCase(selectedFile->getFileName(), newFileName)) {
		return false;
	}

	size_t indexOfExistingFile = group->indexOfFileWithName(newFileName);

	if(indexOfExistingFile != std::numeric_limits<size_t>::max()) {
		int replaceFileResult = wxMessageBox(fmt::format("Group already has a file named '{}'! Would you like to replace it?", newFileName), "Replace File", wxICON_QUESTION | wxYES_NO | wxCANCEL, this);

		if(replaceFileResult != wxYES || !group->removeFile(indexOfExistingFile)) {
			return false;
		}
	}

	size_t fileIndex = group->indexOfFile(*selectedFile);
	std::string previousGroupFileName(selectedFile->getFileName());

	return group->renameFile(*selectedFile, newFileName);
}

bool ProjectPanel::renameSelectedFileInCurrentGroup() {
	wxASSERT(wxIsMainThread());

	std::shared_ptr<Group> group(std::dynamic_pointer_cast<Group>(getCurrentGameFile()));

	if(group == nullptr) {
		WXUtilities::showErrorMessage("Current game file panel does not contain a group.", "Incorrect Panel Type", this);
		return false;
	}

	return renameSelectedFileInGroup(group.get());
}

std::vector<std::shared_ptr<GroupFile>> ProjectPanel::extractFilesFromGroup(const Group * group, const std::vector<std::shared_ptr<GroupFile>> & files) {
	wxASSERT(wxIsMainThread());

	if(!Group::isValid(group)) {
		WXUtilities::showErrorMessage("Invalid group file.", "Invalid Group File", this);
		return {};
	}

	for(const std::shared_ptr<GroupFile> & file : files) {
		if(file->getParentGroup() != group) {
			spdlog::warn("Tried to extract file which did not belong to the provided group.");
			return {};
		}
	}

	std::string basePath(Utilities::getBasePath(group->getFilePath()));

	if(basePath.empty()) {
		basePath = std::filesystem::current_path().string();
	}

	wxDirDialog extractFilesDialog(this, "Select File Extraction Destination Directory", Utilities::getAbsoluteFilePath(basePath, std::filesystem::current_path().string()), wxDD_DIR_MUST_EXIST, wxDefaultPosition, wxDefaultSize, "Extract Files");
	int extractFilesResult = extractFilesDialog.ShowModal();

	if(extractFilesResult == wxID_CANCEL) {
		return {};
	}

	bool overwriteFiles = false;
	std::vector<std::string> existingFileNames;
	std::stringstream existingFileNamesStream;
	std::string destinationDirectoryPath(extractFilesDialog.GetPath());

	for(const std::shared_ptr<GroupFile> & file : files) {
		if(!std::filesystem::exists(std::filesystem::path(Utilities::joinPaths(destinationDirectoryPath, file->getFileName())))) {
			continue;
		}

		existingFileNames.push_back(file->getFileName());

		if(existingFileNamesStream.tellp() != 0) {
			existingFileNamesStream << ", ";
		}

		existingFileNamesStream << '\'' << file->getFileName() << '\'';
	}

	if(!existingFileNames.empty()) {
		bool singleFile = existingFileNames.size() == 1;

		int replaceFilesResult = wxMessageBox(
			fmt::format(
				"File{0} with name{0} {1} already exist{2} in destination directory: '{3}'! Would you like to overwrite {4}?",
				singleFile ? "" : "s",
				existingFileNamesStream.str(),
				singleFile ? "s" : "",
				destinationDirectoryPath,
				singleFile ? "this file" : "these files"
			),
			fmt::format(
				"Overwrite File{}",
				singleFile ? "" : "s"
			),
			wxICON_QUESTION | wxYES_NO | wxCANCEL,
			this
		);

		if(replaceFilesResult == wxYES) {
			overwriteFiles = true;
		}
		else if(replaceFilesResult == wxNO) {
			overwriteFiles = false;
		}
		else if(replaceFilesResult == wxCANCEL) {
			return {};
		}
	}

	std::vector<std::shared_ptr<GroupFile>> extractedFiles;

	for(const std::shared_ptr<GroupFile> & file : files) {
		if(file->writeTo(destinationDirectoryPath, overwriteFiles)) {
			extractedFiles.push_back(file);
		}
	}

	WXUtilities::showInfoMessage(fmt::format("Extracted {} file{} to directory: '{}'.", extractedFiles.size(), extractedFiles.size() == 1 ? "" : "s", destinationDirectoryPath), "Extraction Summary", this);

	return extractedFiles;
}

std::vector<std::shared_ptr<GroupFile>> ProjectPanel::extractSelectedFilesFromGroup(const Group * group) {
	wxASSERT(wxIsMainThread());

	if(!Group::isValid(group)) {
		WXUtilities::showErrorMessage("Invalid group file.", "Invalid Group File", this);
		return {};
	}

	GroupPanel * groupPanel = dynamic_cast<GroupPanel *>(getPanelWithGameFile(group));

	if(groupPanel == nullptr) {
		WXUtilities::showErrorMessage("Current game file panel does not contain a group.", "Incorrect Panel Type", this);
		return {};
	}

	return extractFilesFromGroup(group, groupPanel->getSelectedFiles());
}

std::vector<std::shared_ptr<GroupFile>> ProjectPanel::extractSelectedFilesFromCurrentGroup() {
	wxASSERT(wxIsMainThread());

	std::shared_ptr<const Group> group(std::dynamic_pointer_cast<const Group>(getCurrentGameFile()));

	if(group == nullptr) {
		WXUtilities::showErrorMessage("Current game file panel does not contain a group.", "Incorrect Panel Type", this);
		return {};
	}

	return extractSelectedFilesFromGroup(group.get());
}

std::vector<std::shared_ptr<GroupFile>> ProjectPanel::extractAllFilesFromGroup(const Group * group) {
	wxASSERT(wxIsMainThread());

	return extractFilesFromGroup(group, group->getFiles());
}

std::vector<std::shared_ptr<GroupFile>> ProjectPanel::extractAllFilesFromCurrentGroup() {
	wxASSERT(wxIsMainThread());

	std::shared_ptr<const Group> group(std::dynamic_pointer_cast<const Group>(getCurrentGameFile()));

	if(group == nullptr) {
		WXUtilities::showErrorMessage("Current game file panel does not contain a group.", "Incorrect Panel Type", this);
		return {};
	}

	return extractAllFilesFromGroup(group.get());
}

bool ProjectPanel::closeGameFilePanel(size_t gameFilePanelIndex) {
	wxASSERT(wxIsMainThread());

	std::shared_ptr<GameFile> gameFile(getGameFile(gameFilePanelIndex));

	if(gameFile == nullptr) {
		WXUtilities::showErrorMessage("Panel does not contain a game file.", "Incorrect Panel Type", this);
		return false;
	}

	if(gameFile->isModified()) {
		int saveChangesResult = wxMessageBox("Game file modifications have not been saved! Would you like to save your changes?", "Save Changes", wxICON_QUESTION | wxYES_NO | wxCANCEL, this);

		if(saveChangesResult == wxYES) {
			if(!saveGameFile(gameFile.get())) {
				return false;
			}
		}
		else if(saveChangesResult == wxCANCEL) {
			return false;
		}
	}

	m_gameFilePanelConnections[gameFilePanelIndex].disconnect();
	m_gameFilePanelConnections.erase(m_gameFilePanelConnections.begin() + gameFilePanelIndex);

	const int currentGameFilePanelIndex = m_notebook->GetSelection();

	GameFilePanel * gameFilePanel = getGameFilePanel(gameFilePanelIndex);
	m_notebook->RemovePage(gameFilePanelIndex);
	delete gameFilePanel;
	m_notebook->Refresh();

	if(gameFilePanelIndex == currentGameFilePanelIndex) {
		activeGameFilePanelChanged(*this, getCurrentGameFilePanel());
	}

	return true;
}

bool ProjectPanel::closeCurrentGameFilePanel() {
	wxASSERT(wxIsMainThread());

	int currentGameFilePanelIndex = m_notebook->GetSelection();

	if(currentGameFilePanelIndex == wxNOT_FOUND) {
		return false;
	}

	if(!closeGameFilePanel(currentGameFilePanelIndex)) {
		return false;
	}

	return true;
}

bool ProjectPanel::closeAllGameFilePanels() {
	wxASSERT(wxIsMainThread());

	while(m_notebook->GetPageCount() != 0) {
		if(!closeCurrentGameFilePanel()) {
			return false;
		}
	}

	return true;
}

bool ProjectPanel::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString & filePaths) {
	return openGameFiles(WXUtilities::createItemStringList(filePaths)) != 0u;
}

void ProjectPanel::onNotebookPageChanged(wxBookCtrlEvent & event) {
	activeGameFilePanelChanged(*this, getCurrentGameFilePanel());
}

void ProjectPanel::onGameFileModified(GameFilePanel & gameFilePanel) {
	updateGameFilePanel(indexOfGameFilePanel(&gameFilePanel));

	gameFileModified(*this, gameFilePanel);
}

void ProjectPanel::onGroupFileSelectionChanged(GroupPanel & groupPanel) {
	groupFileSelectionChanged(*this, groupPanel);
}
