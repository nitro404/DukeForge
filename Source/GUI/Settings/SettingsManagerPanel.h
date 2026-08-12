#ifndef _SETTINGS_MANAGER_PANEL_H_
#define _SETTINGS_MANAGER_PANEL_H_

#include <boost/signals2.hpp>
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <functional>
#include <memory>

class DukeForge;
class SettingPanel;
class StringChoiceSettingPanel;

class SettingsManagerPanel final
	: public wxPanel {
public:
	SettingsManagerPanel(std::shared_ptr<DukeForge> dukeForge, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	~SettingsManagerPanel() override;

	bool isModified() const;
	bool isValid() const;
	void reset();
	void discard();
	bool save();

	void updateButtons();

	boost::signals2::signal<void ()> settingsChanged;
	boost::signals2::signal<void ()> settingsReset;
	boost::signals2::signal<void ()> settingsDiscarded;
	boost::signals2::signal<void ()> settingsSaved;

private:
	void onResetDefaultsButtonPressed(wxCommandEvent & event);
	void onDiscardChangesButtonPressed(wxCommandEvent & event);
	void onSaveSettingsButtonPressed(wxCommandEvent & event);
	void onSettingModified(SettingPanel & settingPanel);

	std::shared_ptr<DukeForge> m_dukeForge;
	std::vector<SettingPanel *> m_settingsPanels;
	std::vector<boost::signals2::connection> m_settingModifiedConnections;
	bool m_modified;
	wxButton * m_discardChangesButton;
	wxButton * m_saveSettingsButton;

	SettingsManagerPanel(const SettingsManagerPanel &) = delete;
	const SettingsManagerPanel & operator = (const SettingsManagerPanel &) = delete;
};

#endif // _SETTINGS_MANAGER_PANEL_H_
