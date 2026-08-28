#include "DukeForgeApplication.h"

#include "Application/SettingsManager.h"
#include "Console/Logging/LogSinkWX.h"
#include "Project.h"
#include "WXUtilities.h"

#include <Application/ComponentRegistry.h>
#include <LibraryInformation.h>
#include <Logging/LogSystem.h>
#include <Utilities/FileUtilities.h>

#include <expat.h>
#include <fmt/core.h>
#include <jpeg/jversion.h>
#include <magic_enum/magic_enum.hpp>
#include <png.h>
#include <spdlog/spdlog.h>
#include <tiffio.h>
#include <wx/app.h>
#include <wx/cmdline.h>
#include <wx/progdlg.h>
#include <wx/version.h>

#if !wxUSE_UNICODE || wxUSE_UNICODE_UTF8
	#define PCRE2_CODE_UNIT_WIDTH 8
		typedef char wxRegChar;
#elif wxUSE_UNICODE_UTF16
	#define PCRE2_CODE_UNIT_WIDTH 16
	typedef wchar_t wxRegChar;
#else
	#define PCRE2_CODE_UNIT_WIDTH 32
	typedef wchar_t wxRegChar;
#endif

#include <pcre2.h>

#include <future>

// #include <vld.h>

#define QUOTE(name) #name
#define TOSTRING(macro) QUOTE(macro)

wxDECLARE_EVENT(EVENT_INITIALIZATION_DONE, DukeForgeInitializationDoneEvent);

class DukeForgeInitializationDoneEvent final : public wxEvent {
public:
	DukeForgeInitializationDoneEvent(bool success = true, bool aborted = false)
		: wxEvent(0, EVENT_INITIALIZATION_DONE)
		, m_success(success)
		, m_aborted(aborted) { }

	~DukeForgeInitializationDoneEvent() override { }

	// wxEvent Virtuals
	wxEvent * Clone() const override {
		return new DukeForgeInitializationDoneEvent(*this);
	}

	bool wasSuccessful() const {
		return m_success;
	}

	bool wasAborted() const {
		return m_aborted;
	}

	DECLARE_DYNAMIC_CLASS(DukeForgeInitializationDoneEvent);

private:
	bool m_success;
	bool m_aborted;
};

IMPLEMENT_DYNAMIC_CLASS(DukeForgeInitializationDoneEvent, wxEvent);

wxDEFINE_EVENT(EVENT_INITIALIZATION_DONE, DukeForgeInitializationDoneEvent);

static const std::string BASE_INITIALIZATION_MESSAGE(fmt::format("{} is initializing, please wait...", APPLICATION_NAME));

DukeForgeApplication::DukeForgeApplication()
	: m_dukeForgeFrame(new DukeForgeFrame())
	, m_newDukeForgeFrame(nullptr)
	, m_reloadRequired(false) {
	SetAppDisplayName(APPLICATION_NAME);

	ComponentRegistry::getInstance().registerGlobalComponents();
}

DukeForgeApplication::~DukeForgeApplication() { }

void DukeForgeApplication::initialize() {
	wxASSERT(wxIsMainThread());

	m_dukeForge = std::make_shared<DukeForge>();
	m_logSinkWX = std::make_shared<LogSinkWX>();

	LibraryInformation * libraryInformation = LibraryInformation::getInstance();
	XML_Expat_Version expatVersion = XML_ExpatVersionInfo();
	libraryInformation->addLibrary("LibExpat", fmt::format("{}.{}.{}", expatVersion.major, expatVersion.minor, expatVersion.micro));
	libraryInformation->addLibrary("LibJPEG", JVERSION);
	libraryInformation->addLibrary("Lexilla", LEXILLA_VERSION);
	libraryInformation->addLibrary("NanoSVG", NANOSVG_VERSION);
	libraryInformation->addLibrary("PCRE2", fmt::format("{}.{}", PCRE2_MAJOR, PCRE2_MINOR), TOSTRING(PCRE2_DATE));
	libraryInformation->addLibrary("LibPNG", PNG_LIBPNG_VER_STRING);
	libraryInformation->addLibrary("LibTIFF", TIFFLIB_VERSION_STR_MAJ_MIN_MIC);
	libraryInformation->addLibrary("WebP", WEBP_VERSION);
	libraryInformation->addLibrary("wxWidgets", fmt::format("{}.{}.{}.{}", wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER, wxSUBRELEASE_NUMBER));

	LogSystem::getInstance()->addLogSink(m_logSinkWX);

	SettingsManager * settings = SettingsManager::getInstance();

	settings->load(m_arguments.get());

	if(!settings->analyticsConfirmationAcknowledged) {
		int result = wxMessageBox("Do you consent to the collection of anonymous application usage statistics for the sole purpose of improving application functionality? This information is completely anonymous, non-intrusive, never shared, and extremely helpful to better understand if the application is used at all, what features are used most, or under utilized. Analytics can be enabled or disabled at any time from the application settings screen. Collected information includes:\n- Internal application state and version\n- Application configuration parameters\n- Generalized usage statistics\n- Hardware information\n- Operating system type and version\n- Locale and time zone\n- Coarse location accurate to city only", "Enable Analytics", wxYES_NO | wxCANCEL | wxICON_QUESTION, nullptr);

		if(result == wxYES) {
			settings->segmentAnalyticsEnabled = true;
		}
		else if(result == wxNO) {
			settings->segmentAnalyticsEnabled = false;
		}
		else if(result == wxCANCEL) {
			m_dukeForgeFrame->Close();
			return;
		}

		settings->analyticsConfirmationAcknowledged = true;
	}

	std::unique_ptr<wxProgressDialog> initializingProgressDialog(std::make_unique<wxProgressDialog>(
		"Initializing",
		BASE_INITIALIZATION_MESSAGE,
		m_dukeForge->numberOfInitializationSteps(),
		nullptr,
		wxPD_AUTO_HIDE | wxPD_CAN_ABORT
	));

#if defined(DUKEFORGE_ICON)
	initializingProgressDialog->SetIcon(wxICON(DUKEFORGE_ICON));
#endif // DUKEFORGE_ICON

	initializingProgressDialog->Fit();

	m_dukeForgeInitializationProgressConnection = m_dukeForge->initializationProgress.connect([this, &initializingProgressDialog](uint8_t initializationStep, uint8_t initializationStepCount, std::string description) {
		bool updateResult = initializingProgressDialog->Update(initializationStep, fmt::format("{}\n{}...", BASE_INITIALIZATION_MESSAGE, description));
		initializingProgressDialog->Fit();

		return updateResult;
	});

	Bind(EVENT_INITIALIZATION_DONE, &DukeForgeApplication::onInitializationDone, this);

	bool aborted = false;

	std::future<bool> initializeFuture(std::async(std::launch::async, [this, &aborted]() mutable {
		return m_dukeForge->initialize(m_arguments, &aborted);
	}));

	initializeFuture.wait();

	bool initialized = initializeFuture.get();

	initializingProgressDialog = nullptr;

	if(aborted) {
		QueueEvent(new DukeForgeInitializationDoneEvent(false, true));
	}
	else if(!initialized) {
		QueueEvent(new DukeForgeInitializationDoneEvent(false, false));
	}
	else {
		QueueEvent(new DukeForgeInitializationDoneEvent(true, true));
	}
}

void DukeForgeApplication::reload() {
	wxASSERT(wxIsMainThread());

	m_reloadRequired = true;
	m_newDukeForgeFrame = new DukeForgeFrame();
}

void DukeForgeApplication::displayArgumentHelp() {
	wxASSERT(wxIsMainThread());

	wxMessageBox(DukeForge::getArgumentHelpInformation(), "Argument Information", wxOK | wxICON_INFORMATION);
}

void DukeForgeApplication::showWindow() {
	wxASSERT(wxIsMainThread());

	std::unique_ptr<wxProgressDialog> windowCreationProgressDialog(std::make_unique<wxProgressDialog>(
		"Initializing",
		BASE_INITIALIZATION_MESSAGE + "\nInitializing window...",
		m_dukeForge->numberOfInitializationSteps() + 2,
		nullptr,
		wxPD_AUTO_HIDE | wxPD_CAN_ABORT
	));

#if defined(DUKEFORGE_ICON)
    windowCreationProgressDialog->SetIcon(wxICON(DUKEFORGE_ICON));
#endif // DUKEFORGE_ICON

	windowCreationProgressDialog->Fit();
	windowCreationProgressDialog->Update(m_dukeForge->numberOfInitializationSteps(), windowCreationProgressDialog->GetMessage());

	m_dukeForgeFrameReloadRequestedConnection = m_dukeForgeFrame->reloadRequested.connect(std::bind(&DukeForgeApplication::onReloadRequested, this));
	m_dukeForgeFrame->Bind(wxEVT_CLOSE_WINDOW, &DukeForgeApplication::onFrameClosed, this);
	m_dukeForgeFrame->initialize(m_dukeForge);

	windowCreationProgressDialog->Update(windowCreationProgressDialog->GetValue() + 1, BASE_INITIALIZATION_MESSAGE + "\nApplication initialized!");
	windowCreationProgressDialog->Fit();

	m_dukeForgeFrame->Show(true);
	m_dukeForgeFrame->Raise();

	m_logSinkWX->initialize();

	windowCreationProgressDialog->Update(windowCreationProgressDialog->GetValue() + 1, windowCreationProgressDialog->GetMessage());
}

void DukeForgeApplication::onInitializationDone(DukeForgeInitializationDoneEvent & event) {
	wxASSERT(wxIsMainThread());

	if(event.wasSuccessful()) {
		showWindow();
	}
	else if(event.wasAborted()) {
		spdlog::error("{} initialization cancelled!", APPLICATION_NAME);

		m_dukeForgeFrame->Destroy();
	}
	else {
		spdlog::error("{} initialization failed!", APPLICATION_NAME);

		showWindow();
	}
}

void DukeForgeApplication::onFrameClosed(wxCloseEvent & event) {
	wxASSERT(wxIsMainThread());

	SettingsManager * settings = SettingsManager::getInstance();

	settings->windowPosition = WXUtilities::createPoint(m_dukeForgeFrame->GetPosition());
	settings->windowSize = WXUtilities::createDimension(m_dukeForgeFrame->GetSize());

	m_dukeForgeInitializationProgressConnection.disconnect();
	m_dukeForgeFrameReloadRequestedConnection.disconnect();

	m_dukeForgeFrame->Destroy();

	if(m_reloadRequired) {
		m_reloadRequired = false;
		LogSystem::getInstance()->removeLogSink(m_logSinkWX);
		ComponentRegistry::getInstance().deleteAllComponents();
		m_dukeForgeFrame = m_newDukeForgeFrame;
		m_newDukeForgeFrame = nullptr;
		initialize();
	}
}

bool DukeForgeApplication::OnInit() {
	wxASSERT(wxIsMainThread());

	if(wxAppConsole::argc != 0) {
		m_arguments = std::make_shared<ArgumentParser>(wxAppConsole::argc, wxAppConsole::argv);
	}

	if(m_arguments->hasArgument("?")) {
		displayArgumentHelp();
		return false;
	}

	initialize();

	return true;
}

int DukeForgeApplication::OnExit() {
	wxASSERT(wxIsMainThread());

	LogSystem::getInstance()->removeLogSink(m_logSinkWX);

	m_dukeForge->uninitialize();

	return wxApp::OnExit();
}

void DukeForgeApplication::CleanUp() {
	wxASSERT(wxIsMainThread());

	m_logSinkWX.reset();
	m_dukeForge.reset();

	ComponentRegistry::getInstance().deleteAllGlobalComponents();

	wxApp::CleanUp();
}

void DukeForgeApplication::onReloadRequested() {
	wxASSERT(wxIsMainThread());

	reload();
}

IMPLEMENT_APP(DukeForgeApplication)
