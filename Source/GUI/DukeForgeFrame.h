#ifndef _DUKE_FORGE_FRAME_H_
#define _DUKE_FORGE_FRAME_H_

#include "GUI/Project/ProjectPanel.h"

#include <Signal/SignalConnectionGroup.h>

#include <boost/signals2.hpp>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/bookctrl.h>

#include <memory>

class DukeForge;
class SettingsManagerPanel;

class DukeForgeFrame final : public wxFrame {
public:
	DukeForgeFrame();
	~DukeForgeFrame() override;

	bool isInitialized() const;
	bool initialize(std::shared_ptr<DukeForge> dukeForge);

	bool hasProjectPanel(const ProjectPanel * projectPanel) const;
	size_t indexOfProjectPanel(const ProjectPanel * projectPanel) const;
	size_t numberOfProjectPanels() const;
	ProjectPanel * getProjectPanel(size_t gameFileIndex) const;
	ProjectPanel * getCurrentProjectPanel() const;
	bool hasUnsavedProjects() const;
	bool updateProjectPanel(size_t projectPanelIndex);
	void updateProjectPanelNames();
	bool updateProjectPanelName(size_t projectPanelIndex);
	bool addProjectPanel(std::unique_ptr<ProjectPanel> projectPanel);
	bool closeProjectPanel(size_t index);
	bool closeCurrentProjectPanel();
	bool closeAllProjectPanels();

#if wxUSE_MENUS
	void onGameFileMenuItemPressed(wxCommandEvent & event);
	void onGroupMenuItemPressed(wxCommandEvent & event);
	void onViewMenuItemPressed(wxCommandEvent & event);
	void onHelpMenuItemPressed(wxCommandEvent & event);
#endif // wxUSE_MENUS

	boost::signals2::signal<void ()> reloadRequested;
	boost::signals2::signal<void (ProjectPanel * /* projectPanel */)> activeProjectPanelChanged;

private:
	void requestReload();
	void updateMenu();
	void onNotebookPageChanging(wxBookCtrlEvent & event);
	void onNotebookPageChanged(wxBookCtrlEvent & event);
	void onNewFile(wxCommandEvent & event);
	void onOpenFiles(wxCommandEvent & event);
	void onSaveFile(wxCommandEvent & event);
	void onSaveFileAs(wxCommandEvent & event);
	void onCloseFile(wxCommandEvent & event);
	void onQuit(wxCommandEvent & event);
	void onAbout(wxCommandEvent & event);
	void onSettingsReset();
	void onSettingsSaved();
	void onActiveGameFilePanelChanged(ProjectPanel & projectPanel, GameFilePanel * gameFilePanel);
	void onGameFileModified(ProjectPanel & projectPanel, GameFilePanel & gameFilePanel);
	void onGroupFileSelectionChanged(ProjectPanel & projectPanel, GameFilePanel & gameFilePanel);
	void onActiveProjectPanelChanged(ProjectPanel * projectPanel);

#if wxUSE_MENUS
	wxMenuItem * m_newGameFileMenuItem;
	wxMenuItem * m_openGameFilesMenuItem;
	wxMenuItem * m_saveGameFileMenuItem;
	wxMenuItem * m_saveGameFileAsMenuItem;
	wxMenuItem * m_saveAllGameFilesMenuItem;
	wxMenuItem * m_closeGameFileMenuItem;
	wxMenuItem * m_closeAllGameFilesMenuItem;
	wxMenuItem * m_exitMenuItem;
	wxMenuItem * m_createGroupFromDirectoryMenuItem;
	wxMenuItem * m_addFilesToGroupMenuItem;
	wxMenuItem * m_removeFilesFromGroupMenuItem;
	wxMenuItem * m_replaceFileInGroupMenuItem;
	wxMenuItem * m_renameFileInGroupMenuItem;
	wxMenuItem * m_extractFilesFromGroupMenuItem;
	wxMenuItem * m_extractAllFilesFromGroupMenuItem;
	wxMenuItem * m_resetWindowPositionMenuItem;
	wxMenuItem * m_resetWindowSizeMenuItem;
	wxMenuItem * m_aboutMenuItem;
#endif // wxUSE_MENUS

	bool m_initialized;
	wxNotebook * m_notebook;
	boost::signals2::connection m_activeProjectPanelChangedConnection;
	std::vector<SignalConnectionGroup> m_projectPanelConnections;
	SettingsManagerPanel * m_settingsManagerPanel;
	SignalConnectionGroup m_settingsManagerPanelSignalConnectionGroup;

	DukeForgeFrame(const DukeForgeFrame &) = delete;
	const DukeForgeFrame & operator = (const DukeForgeFrame &) = delete;
};

#endif // _DUKE_FORGE_FRAME_H_
