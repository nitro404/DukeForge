#ifndef _GROUP_PANEL_H_
#define _GROUP_PANEL_H_

#include "GUI/Game/File/GameFilePanel.h"

#include <boost/signals2.hpp>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/gbsizer.h>

class GameFile;
class Group;
class GroupFile;
class MetadataPanel;

class GroupPanel : public GameFilePanel {
public:
	GroupPanel(std::unique_ptr<Group> group, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~GroupPanel() override;

	std::shared_ptr<const Group> getGroup() const;
	std::shared_ptr<Group> getGroup();
	size_t numberOfFilesSelected() const;
	std::vector<std::shared_ptr<GroupFile>> getSelectedFiles() const;
	size_t getTotalSizeOfSelectedFiles() const;
	size_t extractSelectedFiles(const std::string & directoryPath) const;
	void update() override;
	void updateFileInfo();

	// GameFilePanel Virtuals
	std::string getPanelName() const override;

	boost::signals2::signal<void (GroupPanel & /* groupPanel */)> groupFileSelectionChanged;

protected:
	wxFlexGridSizer * m_groupPropertiesSizer;
	wxGridBagSizer * m_groupInfoSizer;

private:
	void onFileSelected(wxCommandEvent & event);

	wxStaticText * m_numberOfFilesText;
	wxStaticText * m_groupSizeText;
	wxStaticText * m_fileExtensionsText;
	wxListBox * m_fileListBox;
	wxStaticBox * m_fileInfoBox;
	MetadataPanel * m_fileInfoPanel;
	wxBoxSizer * m_fileInfoBoxSizer;

	GroupPanel(const GroupPanel &) = delete;
	const GroupPanel & operator = (const GroupPanel &) = delete;
};

#endif // _GROUP_PANEL_H_
