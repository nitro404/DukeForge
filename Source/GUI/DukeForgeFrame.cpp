#include "DukeForgeFrame.h"

#include "Console/ConsolePanel.h"
#include "Application/DukeForge.h"
#include "Application/SettingsManager.h"
#include "Game/File/Group/Group.h"
#include "Game/File/Group/GroupFile.h"
#include "GUI/Game/File/GameFilePanel.h"
#include "GUI/Game/File/GameFilePanelFactoryRegistry.h"
#include "GUI/Game/File/Group/GroupPanel.h"
#include "Project.h"
#include "Project/ProjectPanel.h"
#include "Releases/ReleaseNotesPanel.h"
#include "Settings/SettingsManagerPanel.h"
#include "WXUtilities.h"

#include <Core.h>
#include <LibraryInformation.h>

#include <spdlog/spdlog.h>

DukeForgeFrame::DukeForgeFrame()
	: wxFrame(nullptr, wxID_ANY, APPLICATION_NAME, wxDefaultPosition, wxDefaultSize)
	, m_initialized(false)
#if wxUSE_MENUS
	, m_newGameFileMenuItem(nullptr)
	, m_openGameFilesMenuItem(nullptr)
	, m_saveGameFileMenuItem(nullptr)
	, m_saveGameFileAsMenuItem(nullptr)
	, m_saveAllGameFilesMenuItem(nullptr)
	, m_closeGameFileMenuItem(nullptr)
	, m_closeAllGameFilesMenuItem(nullptr)
	, m_exitMenuItem(nullptr)
	, m_createGroupFromDirectoryMenuItem(nullptr)
	, m_resetWindowPositionMenuItem(nullptr)
	, m_resetWindowSizeMenuItem(nullptr)
	, m_aboutMenuItem(nullptr)
	, m_addFilesToGroupMenuItem(nullptr)
	, m_removeFilesFromGroupMenuItem(nullptr)
	, m_replaceFileInGroupMenuItem(nullptr)
	, m_renameFileInGroupMenuItem(nullptr)
	, m_extractFilesFromGroupMenuItem(nullptr)
	, m_extractAllFilesFromGroupMenuItem(nullptr)
#endif // wxUSE_MENUS
	, m_notebook(nullptr)
	, m_settingsManagerPanel(nullptr) {
	FactoryRegistry & factoryRegistry = FactoryRegistry::getInstance();

	factoryRegistry.setFactory<GameFilePanelFactoryRegistry>([]() {
		return std::make_unique<GameFilePanelFactoryRegistry>();
	});

	SetIcon(wxICON(DUKEFORGE_ICON));

#if wxUSE_MENUS
	wxMenu * gameFileMenu = new wxMenu();
	m_newGameFileMenuItem = new wxMenuItem(gameFileMenu, wxID_NEW, "&New File\tCtrl-N", "Creates a new game file", wxITEM_NORMAL);
	gameFileMenu->Append(m_newGameFileMenuItem);
	m_openGameFilesMenuItem = new wxMenuItem(gameFileMenu, wxID_OPEN, "&Open File(s)\tCtrl-O", "Opens existing game files", wxITEM_NORMAL);
	gameFileMenu->Append(m_openGameFilesMenuItem);
	m_saveGameFileMenuItem = new wxMenuItem(gameFileMenu, wxID_SAVE, "&Save File\tCtrl-S", "Saves changes to the current game file", wxITEM_NORMAL);
	gameFileMenu->Append(m_saveGameFileMenuItem);
	m_saveGameFileAsMenuItem = new wxMenuItem(gameFileMenu, wxID_SAVEAS, "Save File As", "Saves the current game file to a new file", wxITEM_NORMAL);
	gameFileMenu->Append(m_saveGameFileAsMenuItem);
	m_saveAllGameFilesMenuItem = new wxMenuItem(gameFileMenu, wxID_ANY, "Save &All Files", "Saves changes to all open game files", wxITEM_NORMAL);
	gameFileMenu->Append(m_saveAllGameFilesMenuItem);
	m_closeGameFileMenuItem = new wxMenuItem(gameFileMenu, wxID_CLOSE, "Close File\tCtrl-W", "Closes the current game file", wxITEM_NORMAL);
	gameFileMenu->Append(m_closeGameFileMenuItem);
	m_closeAllGameFilesMenuItem = new wxMenuItem(gameFileMenu, wxID_CLOSE_ALL, "Close All Files", "Closes all open game files", wxITEM_NORMAL);
	gameFileMenu->Append(m_closeAllGameFilesMenuItem);
	m_exitMenuItem = new wxMenuItem(gameFileMenu, wxID_EXIT, "E&xit\tCtrl-X", "Close the application", wxITEM_NORMAL);
	gameFileMenu->Append(m_exitMenuItem);
	gameFileMenu->Bind(wxEVT_MENU, &DukeForgeFrame::onGameFileMenuItemPressed, this);

	wxMenu * groupMenu = new wxMenu();
	m_createGroupFromDirectoryMenuItem = new wxMenuItem(groupMenu, wxID_ANY, "Create Group from &Directory", "Creates a new group file from a directory of files", wxITEM_NORMAL);
	groupMenu->Append(m_createGroupFromDirectoryMenuItem);
	m_addFilesToGroupMenuItem = new wxMenuItem(groupMenu, wxID_ANY, "Add Files", "Add files to the current group", wxITEM_NORMAL);
	groupMenu->Append(m_addFilesToGroupMenuItem);
	m_removeFilesFromGroupMenuItem = new wxMenuItem(groupMenu, wxID_ANY, "Remove Files", "Remove files from the current group", wxITEM_NORMAL);
	groupMenu->Append(m_removeFilesFromGroupMenuItem);
	m_replaceFileInGroupMenuItem = new wxMenuItem(groupMenu, wxID_ANY, "Replace File", "Replace the selected file in the current group with another file", wxITEM_NORMAL);
	groupMenu->Append(m_replaceFileInGroupMenuItem);
	m_renameFileInGroupMenuItem = new wxMenuItem(groupMenu, wxID_ANY, "Rename File", "Rename the selected file in the current group to a new file name", wxITEM_NORMAL);
	groupMenu->Append(m_renameFileInGroupMenuItem);
	m_extractFilesFromGroupMenuItem = new wxMenuItem(groupMenu, wxID_ANY, "Extract Files", "Extracts the selected files from the current group", wxITEM_NORMAL);
	groupMenu->Append(m_extractFilesFromGroupMenuItem);
	m_extractAllFilesFromGroupMenuItem = new wxMenuItem(groupMenu, wxID_ANY, "Extract All Files", "Extracts all files from the current group", wxITEM_NORMAL);
	groupMenu->Append(m_extractAllFilesFromGroupMenuItem);
	groupMenu->Bind(wxEVT_MENU, &DukeForgeFrame::onGroupMenuItemPressed, this);

	wxMenu * viewMenu = new wxMenu();
	m_resetWindowPositionMenuItem = new wxMenuItem(viewMenu, wxID_ANY, "Reset Window Position", "Resets the window position", wxITEM_NORMAL);
	viewMenu->Append(m_resetWindowPositionMenuItem);
	m_resetWindowSizeMenuItem = new wxMenuItem(viewMenu, wxID_ANY, "Reset Window Size", "Resets the window size", wxITEM_NORMAL);
	viewMenu->Append(m_resetWindowSizeMenuItem);
	viewMenu->Bind(wxEVT_MENU, &DukeForgeFrame::onViewMenuItemPressed, this);

	wxMenu * helpMenu = new wxMenu();
	helpMenu->Bind(wxEVT_MENU, &DukeForgeFrame::onHelpMenuItemPressed, this);
	m_aboutMenuItem = new wxMenuItem(helpMenu, wxID_ABOUT, "&About\tF1", "Show application information", wxITEM_NORMAL);
	helpMenu->Append(m_aboutMenuItem);
	helpMenu->Bind(wxEVT_MENU, &DukeForgeFrame::onHelpMenuItemPressed, this);

	wxMenuBar * menuBar = new wxMenuBar();
	menuBar->Append(gameFileMenu, "&File");
	menuBar->Append(groupMenu, "&Group");
	menuBar->Append(viewMenu, "&View");
	menuBar->Append(helpMenu, "&Help");

	SetMenuBar(menuBar);
#endif // wxUSE_MENUS
}

DukeForgeFrame::~DukeForgeFrame() {
	m_activeProjectPanelChangedConnection.disconnect();
	m_settingsManagerPanelSignalConnectionGroup.disconnect();
}

bool DukeForgeFrame::isInitialized() const {
	return m_initialized;
}

bool DukeForgeFrame::initialize(std::shared_ptr<DukeForge> dukeForge) {
	if(m_initialized) {
		return true;
	}

	if(dukeForge == nullptr) {
		return false;
	}

	SetPosition(WXUtilities::createWXPoint(SettingsManager::getInstance()->windowPosition));
	SetSize(WXUtilities::createWXSize(SettingsManager::getInstance()->windowSize));
	SetMinSize(WXUtilities::createWXSize(SettingsManager::MINIMUM_WINDOW_SIZE));

	m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP, "Main");

	addProjectPanel(std::make_unique<ProjectPanel>(m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL));

	m_settingsManagerPanel = new SettingsManagerPanel(dukeForge, m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_notebook->AddPage(m_settingsManagerPanel, "Settings");

	m_settingsManagerPanelSignalConnectionGroup = SignalConnectionGroup(
		m_settingsManagerPanel->settingsReset.connect(std::bind(&DukeForgeFrame::onSettingsReset, this)),
		m_settingsManagerPanel->settingsSaved.connect(std::bind(&DukeForgeFrame::onSettingsSaved, this))
	);

	ReleaseNotesPanel * releaseNotesPanel = new ReleaseNotesPanel(m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_notebook->AddPage(releaseNotesPanel, "Release Notes");

	ConsolePanel * consolePanel = new ConsolePanel(m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_notebook->AddPage(consolePanel, "Console");

	updateMenu();

	m_activeProjectPanelChangedConnection = activeProjectPanelChanged.connect(std::bind(&DukeForgeFrame::onActiveProjectPanelChanged, this, std::placeholders::_1));

	if(!dukeForge->isInitialized()) {
		wxMessageBox(
			fmt::format(
				"{} initialization failed!\n"
				"\n"
				"See console for details.",
				APPLICATION_NAME
			),
			"Initialization Failed",
			wxOK | wxICON_ERROR,
			this
		);

		return false;
	}
	else if(dukeForge->didArgumentHandlingFail()) {
		wxMessageBox(
			fmt::format(
				"{} command line argument handling failed!\n"
				"\n"
				"See console for details.",
				APPLICATION_NAME
			),
			"Argument Handling Failure",
			wxOK | wxICON_ERROR,
			this
		);

		wxMessageBox(DukeForge::getArgumentHelpInformation(), "Argument Information", wxOK | wxICON_INFORMATION);
	}

	m_initialized = true;

	return true;
}

bool DukeForgeFrame::hasProjectPanel(const ProjectPanel * projectPanel) const {
	if(projectPanel == nullptr) {
		return false;
	}

	for(size_t i = 0; i < m_notebook->GetPageCount(); i++) {
		if(m_notebook->GetPage(i) == projectPanel) {
			return true;
		}
	}

	return false;
}

size_t DukeForgeFrame::indexOfProjectPanel(const ProjectPanel * projectPanel) const {
	if(projectPanel == nullptr) {
		return false;
	}

	for(size_t i = 0; i < m_notebook->GetPageCount(); i++) {
		if(m_notebook->GetPage(i) == projectPanel) {
			return i;
		}
	}

	return std::numeric_limits<size_t>::max();
}

size_t DukeForgeFrame::numberOfProjectPanels() const {
	size_t projectPanelCount = 0u;

	for(size_t i = 0; i < m_notebook->GetPageCount(); i++) {
		if(dynamic_cast<const ProjectPanel *>(m_notebook->GetPage(i)) != nullptr) {
			projectPanelCount++;
		}
	}

	return projectPanelCount;
}

ProjectPanel * DukeForgeFrame::getProjectPanel(size_t projectPanelIndex) const {
	if(projectPanelIndex >= m_notebook->GetPageCount()) {
		return nullptr;
	}

	return dynamic_cast<ProjectPanel *>(m_notebook->GetPage(projectPanelIndex));
}

ProjectPanel * DukeForgeFrame::getCurrentProjectPanel() const {
	return dynamic_cast<ProjectPanel *>(m_notebook->GetCurrentPage());
}

bool DukeForgeFrame::hasUnsavedProjects() const {
	for(size_t i = 0; i < numberOfProjectPanels(); i++) {
		ProjectPanel * projectPanel = getProjectPanel(i);

		if(projectPanel->hasUnsavedGameFiles()) {
			return true;
		}
	}

	return false;
}

bool DukeForgeFrame::updateProjectPanel(size_t projectPanelIndex) {
	ProjectPanel * projectPanel = getProjectPanel(projectPanelIndex);

	if(projectPanel == nullptr) {
		return false;
	}

	updateProjectPanelName(projectPanelIndex);

	return true;
}

void DukeForgeFrame::updateProjectPanelNames() {
	for(size_t i = 0; i < m_notebook->GetPageCount(); i++) {
		updateProjectPanelName(i);
	}
}

bool DukeForgeFrame::updateProjectPanelName(size_t projectPanelIndex) {
	ProjectPanel * projectPanel = getProjectPanel(projectPanelIndex);

	if(projectPanel == nullptr) {
		return false;
	}

	m_notebook->SetPageText(projectPanelIndex, projectPanel->getPanelName());
	m_notebook->Refresh();

	return true;
}

bool DukeForgeFrame::addProjectPanel(std::unique_ptr<ProjectPanel> projectPanel) {
	if(projectPanel == nullptr || hasProjectPanel(projectPanel.get())) {
		return false;
	}

	size_t projectPanelInsertionIndex = 0u;

	for(size_t i = 0; i < m_notebook->GetPageCount(); i++) {
		if(dynamic_cast<const ProjectPanel *>(m_notebook->GetPage(i)) != nullptr) {
			projectPanelInsertionIndex = i + 1;
		}
	}

	m_projectPanelConnections.push_back(SignalConnectionGroup(
		projectPanel->activeGameFilePanelChanged.connect(std::bind(&DukeForgeFrame::onActiveGameFilePanelChanged, this, std::placeholders::_1, std::placeholders::_2)),
		projectPanel->gameFileModified.connect(std::bind(&DukeForgeFrame::onGameFileModified, this, std::placeholders::_1, std::placeholders::_2)),
		projectPanel->groupFileSelectionChanged.connect(std::bind(&DukeForgeFrame::onGroupFileSelectionChanged, this, std::placeholders::_1, std::placeholders::_2))
	));

	const std::string projectPanelName(projectPanel->getPanelName());

	m_notebook->InsertPage(projectPanelInsertionIndex, projectPanel.release(), projectPanelName);
	m_notebook->ChangeSelection(m_notebook->GetPageCount() - 1);

	return true;
}

bool DukeForgeFrame::closeProjectPanel(size_t projectPanelIndex) {
	ProjectPanel * projectPanel = getProjectPanel(projectPanelIndex);

	if(projectPanel == nullptr) {
		return false;
	}

	if(projectPanel->hasUnsavedGameFiles()) {
		int saveChangesResult = wxMessageBox("Current project has unsaved modifications to open game files. Would you like to save your changes to all open game files before closing the project?", "Save All Changes", wxICON_QUESTION | wxYES_NO | wxCANCEL, this);

		if(saveChangesResult == wxYES) {
			if(!projectPanel->saveAllGameFiles()) {
				return false;
			}
		}
		else if(saveChangesResult == wxCANCEL) {
			return false;
		}
	}

	m_projectPanelConnections[projectPanelIndex].disconnect();
	m_projectPanelConnections.erase(m_projectPanelConnections.begin() + projectPanelIndex);

	const int currentProjectPanelIndex = m_notebook->GetSelection();

	m_notebook->RemovePage(projectPanelIndex);
	delete projectPanel;

	if(projectPanelIndex == currentProjectPanelIndex) {
		activeProjectPanelChanged(getCurrentProjectPanel());
	}

	return true;
}

bool DukeForgeFrame::closeCurrentProjectPanel() {
	int currentProjectPanelIndex = m_notebook->GetSelection();

	if(currentProjectPanelIndex == wxNOT_FOUND) {
		return false;
	}

	if(!closeProjectPanel(currentProjectPanelIndex)) {
		return false;
	}

	return true;
}

bool DukeForgeFrame::closeAllProjectPanels() {
	while(m_notebook->GetPageCount() != 0) {
		if(!closeCurrentProjectPanel()) {
			return false;
		}
	}

	return true;
}

void DukeForgeFrame::requestReload() {
	reloadRequested();

	Close();
}

void DukeForgeFrame::updateMenu() {
	const ProjectPanel * projectPanel = getCurrentProjectPanel();
	const GameFilePanel * gameFilePanel = projectPanel != nullptr ? projectPanel->getCurrentGameFilePanel() : nullptr;
	const GroupPanel * groupPanel = dynamic_cast<const GroupPanel *>(gameFilePanel);
	std::shared_ptr<const Group> group(groupPanel != nullptr ? groupPanel->getGroup() : nullptr);

	m_newGameFileMenuItem->Enable(projectPanel != nullptr);
	m_openGameFilesMenuItem->Enable(projectPanel != nullptr);
	m_saveGameFileMenuItem->Enable(gameFilePanel != nullptr);
	m_saveGameFileAsMenuItem->Enable(gameFilePanel != nullptr);
	m_saveAllGameFilesMenuItem->Enable(projectPanel != nullptr && projectPanel->numberOfGameFilePanels() != 0u);
	m_closeGameFileMenuItem->Enable(gameFilePanel != nullptr);
	m_closeAllGameFilesMenuItem->Enable(projectPanel != nullptr && projectPanel->numberOfGameFilePanels() != 0u);
	m_createGroupFromDirectoryMenuItem->Enable(projectPanel != nullptr);
	m_addFilesToGroupMenuItem->Enable(groupPanel != nullptr);
	m_removeFilesFromGroupMenuItem->Enable(groupPanel != nullptr && groupPanel->numberOfFilesSelected() != 0u);
	m_replaceFileInGroupMenuItem->Enable(groupPanel != nullptr && groupPanel->numberOfFilesSelected() == 1u);
	m_renameFileInGroupMenuItem->Enable(groupPanel != nullptr && groupPanel->numberOfFilesSelected() == 1u);
	m_extractFilesFromGroupMenuItem->Enable(groupPanel != nullptr && groupPanel->numberOfFilesSelected() != 0u);
	m_extractAllFilesFromGroupMenuItem->Enable(group != nullptr && group->numberOfFiles() != 0u);
}

#if wxUSE_MENUS
void DukeForgeFrame::onGameFileMenuItemPressed(wxCommandEvent & event) {
	ProjectPanel * currentProjectPanel = getCurrentProjectPanel();

	if(currentProjectPanel == nullptr) {
		return;
	}

	if(event.GetId() == m_newGameFileMenuItem->GetId()) {
		currentProjectPanel->newGameFile();
	}
	else if(event.GetId() == m_openGameFilesMenuItem->GetId()) {
		currentProjectPanel->openGameFiles();
	}
	else if(event.GetId() == m_saveGameFileMenuItem->GetId()) {
		currentProjectPanel->saveCurrentGameFile();
	}
	else if(event.GetId() == m_saveGameFileAsMenuItem->GetId()) {
		currentProjectPanel->saveCurrentGameFileAs();
	}
	else if(event.GetId() == m_saveAllGameFilesMenuItem->GetId()) {
		currentProjectPanel->saveAllGameFiles();
	}
	else if(event.GetId() == m_closeGameFileMenuItem->GetId()) {
		currentProjectPanel->closeCurrentGameFilePanel();
	}
	else if(event.GetId() == m_closeAllGameFilesMenuItem->GetId()) {
		currentProjectPanel->closeAllGameFilePanels();
	}
	else if(event.GetId() == m_exitMenuItem->GetId()) {
		Close();
	}
}

void DukeForgeFrame::onGroupMenuItemPressed(wxCommandEvent & event) {
	ProjectPanel * currentProjectPanel = getCurrentProjectPanel();

	if(currentProjectPanel == nullptr) {
		return;
	}

	if(event.GetId() == m_createGroupFromDirectoryMenuItem->GetId()) {
		currentProjectPanel->createGroupFromDirectory();
	}
	else if(event.GetId() == m_addFilesToGroupMenuItem->GetId()) {
		currentProjectPanel->addFilesToCurrentGroup();
	}
	else if(event.GetId() == m_removeFilesFromGroupMenuItem->GetId()) {
		currentProjectPanel->removeSelectedFilesFromCurrentGroup();
	}
	else if(event.GetId() == m_replaceFileInGroupMenuItem->GetId()) {
		currentProjectPanel->replaceSelectedFileInCurrentGroup();
	}
	else if(event.GetId() == m_renameFileInGroupMenuItem->GetId()) {
		currentProjectPanel->renameSelectedFileInCurrentGroup();
	}
	else if(event.GetId() == m_extractFilesFromGroupMenuItem->GetId()) {
		currentProjectPanel->extractSelectedFilesFromCurrentGroup();
	}
	else if(event.GetId() == m_extractAllFilesFromGroupMenuItem->GetId()) {
		currentProjectPanel->extractAllFilesFromCurrentGroup();
	}
}

void DukeForgeFrame::onViewMenuItemPressed(wxCommandEvent & event) {
	SettingsManager * settings = SettingsManager::getInstance();

	if(event.GetId() == m_resetWindowPositionMenuItem->GetId()) {
		settings->windowPosition.x = 0;
		settings->windowPosition.y = 0;
		SetPosition(WXUtilities::createWXPoint(settings->windowPosition));
	}
	else if(event.GetId() == m_resetWindowSizeMenuItem->GetId()) {
		settings->windowSize = SettingsManager::DEFAULT_WINDOW_SIZE;
		SetSize(WXUtilities::createWXSize(settings->windowSize));
	}
}

void DukeForgeFrame::onHelpMenuItemPressed(wxCommandEvent & event) {
	if(event.GetId() == m_aboutMenuItem->GetId()) {
		wxMessageBox(
			fmt::format(
				"{} {} ({})\n"
				"Created by: Kevin Scroggins\n"
				"\n"
				"Library Information:\n"
				"{}",
				APPLICATION_NAME,
				APPLICATION_VERSION,
				APPLICATION_COMMIT_HASH,
				LibraryInformation::getInstance()->getLibraryInformationString()
			),
			"About",
			wxOK | wxICON_INFORMATION,
			this
		);
	}
}
#endif // wxUSE_MENUS

void DukeForgeFrame::onNotebookPageChanging(wxBookCtrlEvent & event) {
	wxWindow * currentPage = m_notebook->GetPage(m_notebook->GetSelection());

	if(dynamic_cast<SettingsManagerPanel *>(currentPage) != nullptr) {
		SettingsManagerPanel * settingsManagerPanel = static_cast<SettingsManagerPanel *>(currentPage);

		if(!settingsManagerPanel->isModified()) {
			return;
		}

		int result = wxMessageBox("You have unsaved settings modifications.\nWould you like to save settings and re-load the application, or discard your changes?", "Save Settings", wxYES_NO | wxCANCEL | wxICON_INFORMATION, this);

		if(result == wxYES) {
			settingsManagerPanel->save();
			event.Veto();
		}
		else if(result == wxNO) {
			static_cast<SettingsManagerPanel *>(currentPage)->discard();
		}
		else if(result == wxCANCEL) {
			event.Veto();
		}
	}
}

void DukeForgeFrame::onNotebookPageChanged(wxBookCtrlEvent & event) {
	wxWindow * currentPage = m_notebook->GetPage(m_notebook->GetSelection());

	if(dynamic_cast<SettingsManagerPanel *>(currentPage) != nullptr) {
		static_cast<SettingsManagerPanel *>(currentPage)->discard();
	}
	else if(dynamic_cast<ReleaseNotesPanel *>(currentPage) != nullptr) {
		static_cast<ReleaseNotesPanel *>(currentPage)->load();
	}
}

void DukeForgeFrame::onSettingsReset() {
	requestReload();
}

void DukeForgeFrame::onSettingsSaved() {
	requestReload();
}

void DukeForgeFrame::onActiveGameFilePanelChanged(ProjectPanel & projectPanel, GameFilePanel * gameFilePanel) {
	updateMenu();
}

void DukeForgeFrame::onGameFileModified(ProjectPanel & projectPanel, GameFilePanel & gameFilePanel) {
	updateMenu();
}

void DukeForgeFrame::onGroupFileSelectionChanged(ProjectPanel & projectPanel, GameFilePanel & gameFilePanel) {
	updateMenu();
}

void DukeForgeFrame::onActiveProjectPanelChanged(ProjectPanel * projectPanel) {
	updateMenu();
}
