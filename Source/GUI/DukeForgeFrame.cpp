#include "DukeForgeFrame.h"

#include "Console/ConsolePanel.h"
#include "Application/DukeForge.h"
#include "Application/SettingsManager.h"
#include "Group/GroupEditorPanel.h"
#include "Project.h"
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
	, m_resetWindowPositionMenuItem(nullptr)
	, m_resetWindowSizeMenuItem(nullptr)
#endif // wxUSE_MENUS
	, m_notebook(nullptr)
	, m_settingsManagerPanel(nullptr) {
	SetIcon(wxICON(DUKEFORGE_ICON));

#if wxUSE_MENUS
	wxMenu * fileMenu = new wxMenu();
	fileMenu->Append(wxID_EXIT, "E&xit\tAlt-X", "Close the application");

	wxMenu * viewMenu = new wxMenu();
	m_resetWindowPositionMenuItem = new wxMenuItem(viewMenu, wxID_ANY, "Reset Window Position", "Resets the window position", wxITEM_NORMAL);
	viewMenu->Bind(wxEVT_MENU, &DukeForgeFrame::onMenuBarItemPressed, this);
	m_resetWindowSizeMenuItem = new wxMenuItem(viewMenu, wxID_ANY, "Reset Window Size", "Resets the window size", wxITEM_NORMAL);
	viewMenu->Append(m_resetWindowPositionMenuItem);
	viewMenu->Append(m_resetWindowSizeMenuItem);

	wxMenu * helpMenu = new wxMenu();
	helpMenu->Append(wxID_ABOUT, "&About\tF1", "Show application information");

	wxMenuBar * menuBar = new wxMenuBar();
	menuBar->Append(fileMenu, "&File");
	menuBar->Append(viewMenu, "&View");
	menuBar->Append(helpMenu, "&Help");

	SetMenuBar(menuBar);
#endif // wxUSE_MENUS
}

DukeForgeFrame::~DukeForgeFrame() {
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

	GroupEditorPanel * groupEditorPanel = new GroupEditorPanel(m_notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_notebook->AddPage(groupEditorPanel, "Group Editor");

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

#if wxUSE_MENUS
void DukeForgeFrame::onMenuBarItemPressed(wxCommandEvent & event) {
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

void DukeForgeFrame::onQuit(wxCommandEvent& WXUNUSED(event)) {
	Close();
}

void DukeForgeFrame::onAbout(wxCommandEvent& WXUNUSED(event)) {
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

void DukeForgeFrame::onSettingsReset() {
	requestReload();
}

void DukeForgeFrame::onSettingsSaved() {
	requestReload();
}

void DukeForgeFrame::requestReload() {
	reloadRequested();

	Close();
}

wxBEGIN_EVENT_TABLE(DukeForgeFrame, wxFrame)
	EVT_MENU(wxID_EXIT, DukeForgeFrame::onQuit)
	EVT_MENU(wxID_ABOUT, DukeForgeFrame::onAbout)
wxEND_EVENT_TABLE()
