#ifndef _PROJECT_PANEL_H_
#define _PROJECT_PANEL_H_

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
#include <string>

class GameFile;
class GameFilePanel;
class Group;
class GroupFile;
class GroupPanel;
class SignalConnectionGroup;

class ProjectPanel final : public wxPanel {
public:
	ProjectPanel(wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~ProjectPanel() override;

	std::string getPanelName() const;
	bool hasGameFilePanel(const GameFilePanel * gameFilePanel) const;
	bool hasPanelWithGameFile(const GameFile * gameFile) const;
	bool hasPanelWithGameFilePath(const std::string & filePath) const;
	size_t indexOfGameFilePanel(const GameFilePanel * gameFilePanel) const;
	size_t indexOfPanelWithGameFile(const GameFile * gameFile) const;
	size_t indexOfPanelWithGameFileFilePath(const std::string & filePath) const;
	size_t numberOfGameFilePanels() const;
	GameFilePanel * getGameFilePanel(size_t gameFileIndex) const;
	GameFilePanel * getPanelWithGameFile(const GameFile * gameFile) const;
	GameFilePanel * getPanelWithGameFileFilePath(const std::string & filePath) const;
	GameFilePanel * getCurrentGameFilePanel() const;
	std::shared_ptr<GameFile> getGameFile(size_t gameFilePanelIndex) const;
	std::shared_ptr<GameFile> getCurrentGameFile() const;
	bool hasUnsavedGameFiles() const;

	bool updateGameFilePanel(size_t gameFilePanelIndex);
	void updateGameFilePanelNames();
	bool updateGameFilePanelName(size_t gameFilePanelIndex);

	void addGameFilePanel(GameFilePanel * gameFilePanel);
	bool newGameFile();
	bool openGameFile(const std::string & filePath);
	size_t openGameFiles();
	bool createGroupFromDirectory(const std::string & directoryPath);
	bool createGroupFromDirectory();
	bool saveGameFile(GameFile * gameFile);
	bool saveCurrentGameFile();
	bool saveGameFileAs(GameFile * gameFile);
	bool saveCurrentGameFileAs();
	bool saveAllGameFiles();
	size_t addFilesToGroup(Group * group);
	size_t addFilesToCurrentGroup();
	size_t removeSelectedFilesFromGroup(Group * group);
	size_t removeSelectedFilesFromCurrentGroup();
	bool replaceSelectedFileInGroup(Group * group);
	bool replaceSelectedFileInCurrentGroup();
	bool renameSelectedFileInGroup(Group * group);
	bool renameSelectedFileInCurrentGroup();
	std::vector<std::shared_ptr<GroupFile>> extractFilesFromGroup(const Group * group, const std::vector<std::shared_ptr<GroupFile>> & files);
	std::vector<std::shared_ptr<GroupFile>> extractSelectedFilesFromGroup(const Group * group);
	std::vector<std::shared_ptr<GroupFile>> extractSelectedFilesFromCurrentGroup();
	std::vector<std::shared_ptr<GroupFile>> extractAllFilesFromGroup(const Group * group);
	std::vector<std::shared_ptr<GroupFile>> extractAllFilesFromCurrentGroup();
	bool closeGameFilePanel(size_t index);
	bool closeCurrentGameFilePanel();
	bool closeAllGameFilePanels();

	boost::signals2::signal<void (ProjectPanel & /* projectPanel */, GameFilePanel * /* gameFilePanel */)> activeGameFilePanelChanged;
	boost::signals2::signal<void (ProjectPanel & /* projectPanel */, GameFilePanel & /* gameFilePanel */)> gameFileModified;
	boost::signals2::signal<void (ProjectPanel & /* projectPanel */, GameFilePanel & /* gameFilePanel */)> groupFileSelectionChanged;

private:
	void onNotebookPageChanged(wxBookCtrlEvent & event);
	void onGameFileModified(GameFilePanel & gameFilePanel);
	void onGroupFileSelectionChanged(GroupPanel & groupPanel);

	std::vector<SignalConnectionGroup> m_gameFilePanelConnections;
	wxNotebook * m_notebook;

	ProjectPanel(const ProjectPanel &) = delete;
	const ProjectPanel & operator = (const ProjectPanel &) = delete;
};

#endif // _PROJECT_PANEL_H_
