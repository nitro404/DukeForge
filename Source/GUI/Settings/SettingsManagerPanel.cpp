#include "SettingsManagerPanel.h"

#include "../SettingPanel.h"
#include "../WXUtilities.h"
#include "Application/DukeForge.h"
#include "Application/SettingsManager.h"
#include "Project.h"

#include <Logging/LogSystem.h>

#include <wx/gbsizer.h>
#include <wx/wrapsizer.h>

#include <sstream>

SettingsManagerPanel::SettingsManagerPanel(std::shared_ptr<DukeForge> dukeForge, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style)
	: wxPanel(parent, windowID, position, size, style, "Settings")
	, m_dukeForge(dukeForge)
	, m_modified(false)
	, m_discardChangesButton(nullptr)
	, m_saveSettingsButton(nullptr) {
	wxASSERT(wxIsMainThread());

	SettingsManager * settings = SettingsManager::getInstance();

	Freeze();

	const int wrapSizerOrientation = wxHORIZONTAL;

	wxWrapSizer * generalSettingsSizer = new wxWrapSizer(wrapSizerOrientation);
	wxStaticBox * generalSettingsBox = new wxStaticBox(this, wxID_ANY, "General", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT, "General");
	generalSettingsBox->SetOwnFont(generalSettingsBox->GetFont().MakeBold());

	wxPanel * generalSettingsPanel = new wxPanel(generalSettingsBox, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	m_settingsPanels.push_back(SettingPanel::createEnumSettingPanel<spdlog::level::level_enum>(std::bind(&LogSystem::getLevel, LogSystem::getInstance()), std::bind(&LogSystem::setLevel, LogSystem::getInstance(), std::placeholders::_1), LogSystem::DEFAULT_LEVEL, "Log Level", generalSettingsPanel, generalSettingsSizer, { spdlog::level::level_enum::off, spdlog::level::level_enum::n_levels }));
	m_settingsPanels.push_back(SettingPanel::createStringSettingPanel(settings->dataDirectoryPath, SettingsManager::DEFAULT_DATA_DIRECTORY_PATH, "Data Directory Path", generalSettingsPanel, generalSettingsSizer, 1));
	m_settingsPanels.push_back(SettingPanel::createStringSettingPanel(settings->timeZoneDataDirectoryName, SettingsManager::DEFAULT_TIME_ZONE_DATA_DIRECTORY_NAME, "Time Zone Data Directory Name", generalSettingsPanel, generalSettingsSizer, 1));
	m_settingsPanels.push_back(SettingPanel::createStringSettingPanel(settings->curlDataDirectoryName, SettingsManager::DEFAULT_CURL_DATA_DIRECTORY_NAME, "cURL Data Directory Name", generalSettingsPanel, generalSettingsSizer));

	wxWrapSizer * downloadsSettingsSizer = new wxWrapSizer(wrapSizerOrientation);
	wxStaticBox * downloadsSettingsBox = new wxStaticBox(this, wxID_ANY, "Downloads", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT, "Downloads");
	downloadsSettingsBox->SetOwnFont(downloadsSettingsBox->GetFont().MakeBold());

	wxPanel * downloadsSettingsPanel = new wxPanel(downloadsSettingsBox, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	m_settingsPanels.push_back(SettingPanel::createChronoSettingPanel(settings->connectionTimeout, SettingsManager::DEFAULT_CONNECTION_TIMEOUT, "Connection Timeout", downloadsSettingsPanel, downloadsSettingsSizer));
	m_settingsPanels.push_back(SettingPanel::createChronoSettingPanel(settings->networkTimeout, SettingsManager::DEFAULT_NETWORK_TIMEOUT, "Network Timeout", downloadsSettingsPanel, downloadsSettingsSizer));
	m_settingsPanels.push_back(SettingPanel::createChronoSettingPanel(settings->transferTimeout, SettingsManager::DEFAULT_TRANSFER_TIMEOUT, "Transfer Timeout", downloadsSettingsPanel, downloadsSettingsSizer));
	m_settingsPanels.push_back(SettingPanel::createBooleanSettingPanel(settings->verboseRequestLogging, SettingsManager::DEFAULT_VERBOSE_REQUEST_LOGGING, "Verbose Request Logging", downloadsSettingsPanel, downloadsSettingsSizer));
	m_settingsPanels.push_back(SettingPanel::createBooleanSettingPanel(settings->downloadThrottlingEnabled, SettingsManager::DEFAULT_DOWNLOAD_THROTTLING_ENABLED, "Download Throttling", downloadsSettingsPanel, downloadsSettingsSizer));
	m_settingsPanels.push_back(SettingPanel::createChronoSettingPanel(settings->cacertUpdateFrequency, SettingsManager::DEFAULT_TIME_ZONE_DATA_UPDATE_FREQUENCY, "CACert Update Frequency", downloadsSettingsPanel, downloadsSettingsSizer));

	wxWrapSizer * analyticsSettingsSizer = new wxWrapSizer(wrapSizerOrientation);
	wxStaticBox * analyticsSettingsBox = new wxStaticBox(this, wxID_ANY, "Analytics", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT, "Analytics");
	analyticsSettingsBox->SetOwnFont(analyticsSettingsBox->GetFont().MakeBold());

	wxPanel * analyticsSettingsPanel = new wxPanel(analyticsSettingsBox, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	m_settingsPanels.push_back(SettingPanel::createBooleanSettingPanel(settings->segmentAnalyticsEnabled, SettingsManager::DEFAULT_SEGMENT_ANALYTICS_ENABLED, "Segment Analytics Enabled", analyticsSettingsPanel, analyticsSettingsSizer));
	m_settingsPanels.push_back(SettingPanel::createStringSettingPanel(settings->segmentAnalyticsDataFileName, SettingsManager::DEFAULT_SEGMENT_ANALYTICS_DATA_FILE_NAME, "Analytics Data File Name", analyticsSettingsPanel, analyticsSettingsSizer));

	wxPanel * actionsPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);

	m_saveSettingsButton = new wxButton(actionsPanel, wxID_ANY, "Save Settings", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, "Save Settings");
	m_saveSettingsButton->Bind(wxEVT_BUTTON, &SettingsManagerPanel::onSaveSettingsButtonPressed, this);
	m_saveSettingsButton->Disable();

	m_discardChangesButton = new wxButton(actionsPanel, wxID_ANY, "Discard Changes", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, "Discard Changes");
	m_discardChangesButton->Bind(wxEVT_BUTTON, &SettingsManagerPanel::onDiscardChangesButtonPressed, this);
	m_discardChangesButton->Disable();

	wxButton * resetDefaultsButton = new wxButton(actionsPanel, wxID_ANY, "Reset Defaults", wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, "Reset Defaults");
	resetDefaultsButton->Bind(wxEVT_BUTTON, &SettingsManagerPanel::onResetDefaultsButtonPressed, this);

	int border = 5;

	generalSettingsPanel->SetSizerAndFit(generalSettingsSizer);
	downloadsSettingsPanel->SetSizerAndFit(downloadsSettingsSizer);
	analyticsSettingsPanel->SetSizerAndFit(analyticsSettingsSizer);

	wxBoxSizer * generalSettingsBoxSizer = new wxBoxSizer(wxHORIZONTAL);
	generalSettingsBoxSizer->Add(generalSettingsPanel, 1, wxEXPAND | wxALL, 20);
	generalSettingsBox->SetSizer(generalSettingsBoxSizer);

	wxBoxSizer * downloadsSettingsBoxSizer = new wxBoxSizer(wxHORIZONTAL);
	downloadsSettingsBoxSizer->Add(downloadsSettingsPanel, 1, wxEXPAND | wxALL, 20);
	downloadsSettingsBox->SetSizer(downloadsSettingsBoxSizer);

	wxBoxSizer * analyticsSettingsBoxSizer = new wxBoxSizer(wxHORIZONTAL);
	analyticsSettingsBoxSizer->Add(analyticsSettingsPanel, 1, wxEXPAND | wxALL, 20);
	analyticsSettingsBox->SetSizer(analyticsSettingsBoxSizer);

	wxWrapSizer * actionsSizer = new wxWrapSizer(wxHORIZONTAL);
	actionsSizer->Add(m_saveSettingsButton, 1, wxEXPAND | wxHORIZONTAL, border);
	actionsSizer->Add(m_discardChangesButton, 1, wxEXPAND | wxHORIZONTAL, border);
	actionsSizer->Add(resetDefaultsButton, 1, wxEXPAND | wxHORIZONTAL, border);
	actionsPanel->SetSizer(actionsSizer);

	wxGridBagSizer * settingsPanelSizer = new wxGridBagSizer(border, border);
	settingsPanelSizer->Add(generalSettingsBox, wxGBPosition(0, 0), wxGBSpan(1, 2), wxEXPAND | wxALL, border);
	settingsPanelSizer->Add(downloadsSettingsBox, wxGBPosition(1, 0), wxGBSpan(1, 2), wxEXPAND | wxALL, border);
	settingsPanelSizer->Add(analyticsSettingsBox, wxGBPosition(2, 0), wxGBSpan(1, 2), wxEXPAND | wxALL, border);
	settingsPanelSizer->Add(actionsPanel, wxGBPosition(3, 0), wxGBSpan(1, 2), wxEXPAND | wxHORIZONTAL, border);
	settingsPanelSizer->AddGrowableRow(0, 2);
	settingsPanelSizer->AddGrowableRow(1, 3);
	settingsPanelSizer->AddGrowableRow(2, 2);
	settingsPanelSizer->AddGrowableCol(0, 3);
	settingsPanelSizer->AddGrowableCol(1, 1);
	SetSizerAndFit(settingsPanelSizer);

	Thaw();

	for(SettingPanel * settingPanel : m_settingsPanels) {
		m_settingModifiedConnections.push_back(settingPanel->settingModified.connect(std::bind(&SettingsManagerPanel::onSettingModified, this, std::placeholders::_1)));
	}
}

SettingsManagerPanel::~SettingsManagerPanel() {
	for(boost::signals2::connection & settingModifiedConnection : m_settingModifiedConnections) {
		settingModifiedConnection.disconnect();
	}
}

bool SettingsManagerPanel::isModified() const {
	return m_modified;
}

bool SettingsManagerPanel::isValid() const {
	for(const SettingPanel * settingPanel : m_settingsPanels) {
		if(!settingPanel->isValid()) {
			return false;
		}
	}

	return true;
}

void SettingsManagerPanel::reset() {
	SettingsManager * settings = SettingsManager::getInstance();

	settings->reset();

	m_modified = false;

	updateButtons();
	settingsReset();
}

void SettingsManagerPanel::discard() {
	for(SettingPanel * settingPanel : m_settingsPanels) {
		settingPanel->discard();
	}

	m_modified = false;

	updateButtons();
	settingsDiscarded();
}

bool SettingsManagerPanel::save() {
	wxASSERT(wxIsMainThread());

	size_t numberOfInvalidSettings = 0;
	std::stringstream invalidSettingPanelNames;

	for(const SettingPanel * settingPanel : m_settingsPanels) {
		if(!settingPanel->isValid()) {
			numberOfInvalidSettings++;

			if(invalidSettingPanelNames.tellp() != 0) {
				invalidSettingPanelNames << ", ";
			}

			invalidSettingPanelNames << "'" << settingPanel->getName() << "'";
		}
	}

	if(numberOfInvalidSettings != 0) {
		wxMessageBox(
			fmt::format(
				"Failed to validate {0} settings values!\n"
				"\n"
				"The following setting{1} {2} invalid value{1}: {3}",
				APPLICATION_NAME,
				numberOfInvalidSettings == 1 ? "" : "s",
				numberOfInvalidSettings == 1 ? "has an" : "have",
				invalidSettingPanelNames.str()
			),
			"Invalid Settings",
			wxOK | wxICON_WARNING,
			this
		);

		return false;
	}

	for(SettingPanel * settingPanel : m_settingsPanels) {
		if(!settingPanel->save()) {
			return false;
		}
	}

	SettingsManager * settings = SettingsManager::getInstance();

	if(!settings->save()) {
		wxMessageBox("Failed to save settings!", "Save Failed", wxOK | wxICON_ERROR, this);
		return false;
	}

	m_modified = false;

	updateButtons();
	settingsSaved();

	return true;
}

void SettingsManagerPanel::updateButtons() {
	wxASSERT(wxIsMainThread());

	WXUtilities::setButtonEnabled(m_discardChangesButton, m_modified);
	WXUtilities::setButtonEnabled(m_saveSettingsButton, m_modified);
}

void SettingsManagerPanel::onResetDefaultsButtonPressed(wxCommandEvent & event) {
	int resetDefaultsResult = wxMessageBox("Are you sure you want to reset to default settings? Application will automatically re-load after settings reset.", "Reset Settings", wxYES_NO | wxCANCEL | wxICON_WARNING, this);

	if(resetDefaultsResult == wxYES) {
		reset();
	}
}

void SettingsManagerPanel::onDiscardChangesButtonPressed(wxCommandEvent & event) {
	discard();
}

void SettingsManagerPanel::onSaveSettingsButtonPressed(wxCommandEvent & event) {
	save();
}

void SettingsManagerPanel::onSettingModified(SettingPanel & settingPanel) {
	if(settingPanel.isModified()) {
		m_modified = true;

		updateButtons();
		settingsChanged();
	}
}
