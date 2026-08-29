#include "DukeForge.h"

#include "Game/File/GameFileFactoryRegistry.h"
#include "Project.h"
#include "SettingsManager.h"

#include <Factory/FactoryRegistry.h>
#include <LibraryInformation.h>
#include <Network/HTTPService.h>
#include <Platform/TimeZoneDataManager.h>
#include <Utilities/FileUtilities.h>
#include <Utilities/StringUtilities.h>

#include <gcem.hpp>
#include <jdksmidi/version.h>
#include <sndfile.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <cstdio>

static constexpr uint8_t NUMBER_OF_INITIALIZATION_STEPS = 4;

DukeForge::DukeForge()
	: Application()
	, m_initialized(false)
	, m_initializing(false)
	, m_initializationAborted(false)
	, m_initializationStep(0u)
	, m_argumentHandlingFailed(false) {
	FactoryRegistry & factoryRegistry = FactoryRegistry::getInstance();

	factoryRegistry.setFactory<SettingsManager>([]() {
		return std::make_unique<SettingsManager>();
	});

	factoryRegistry.setFactory<GameFileFactoryRegistry>([]() {
		return std::make_unique<GameFileFactoryRegistry>();
	});

	LibraryInformation * libraryInformation = LibraryInformation::getInstance();
	libraryInformation->addLibrary("CSS-Color-Parser", CSS_COLOR_PARSER_VERSION);
	libraryInformation->addLibrary("GCE-Math", fmt::format("{}.{}.{}", GCEM_VERSION_MAJOR, GCEM_VERSION_MINOR, GCEM_VERSION_PATCH));
	libraryInformation->addLibrary("JDKSMIDI", jdksmidi::LibraryVersion);

	std::string_view libSndFileVersion(sf_version_string());
	size_t versionStartIndex = libSndFileVersion.find_first_of("0123456789");
	if(versionStartIndex == std::string::npos) {
		versionStartIndex = 0;
	}
	libraryInformation->addLibrary("LibSndFile", std::string(libSndFileVersion.substr(versionStartIndex, libSndFileVersion.length() - versionStartIndex)));
}

DukeForge::~DukeForge() { }

bool DukeForge::isInitialized() const {
	return m_initialized;
}

bool DukeForge::isInitializing() const {
	return m_initializing;
}

bool DukeForge::wasInitializationAborted() const {
	return m_initializationAborted;
}

uint8_t DukeForge::numberOfInitializationSteps() const {
	return NUMBER_OF_INITIALIZATION_STEPS;
}

bool DukeForge::notifyInitializationProgress(const std::string & description) {
	initializationProgress(m_initializationStep++, NUMBER_OF_INITIALIZATION_STEPS, description);

	return !m_initializationAborted;
}

bool DukeForge::initialize(int argc, char * argv[]) {
	std::shared_ptr<ArgumentParser> arguments;

	if(argc != 0) {
		arguments = std::make_shared<ArgumentParser>(argc, argv);
	}

	return initialize(arguments);
}

bool DukeForge::initialize(std::shared_ptr<ArgumentParser> arguments) {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	if(m_initialized || m_initializing) {
		return false;
	}

	std::chrono::time_point<std::chrono::steady_clock> initializeSteadyStartTimePoint(std::chrono::steady_clock::now());
	m_initializing = true;
	m_initializationStep = 0;
	m_argumentHandlingFailed = false;

	if(!notifyInitializationProgress("Parsing Arguments")) {
		m_initializing = false;
		return false;
	}

	if(arguments != nullptr) {
		m_arguments = arguments;
	}

	if(!notifyInitializationProgress("Loading Settings")) {
		m_initializing = false;
		return false;
	}

	SettingsManager * settings = SettingsManager::getInstance();

	if(!settings->isLoaded()) {
		settings->load(m_arguments.get());
	}

	if(!notifyInitializationProgress("Initializing HTTP Service")) {
		m_initializing = false;
		return false;
	}

	HTTPConfiguration configuration = {
		Utilities::joinPaths(settings->dataDirectoryPath, settings->curlDataDirectoryName),
		"",
		settings->connectionTimeout,
		settings->networkTimeout,
		settings->transferTimeout
	};

	HTTPService * httpService = HTTPService::getInstance();

	if(!httpService->initialize(configuration)) {
		spdlog::error("Failed to initialize HTTP service!");
		return false;
	}

	httpService->setUserAgent(Utilities::toPascalCase(APPLICATION_NAME) + "/" + APPLICATION_VERSION);
	httpService->setVerboseLoggingEnabled(settings->verboseRequestLogging);

	if(!settings->downloadThrottlingEnabled || !settings->cacertLastDownloadedTimestamp.has_value() || std::chrono::system_clock::now() - settings->cacertLastDownloadedTimestamp.value() > settings->cacertUpdateFrequency) {
		if(httpService->updateCertificateAuthorityCertificateStoreFileAndWait()) {
			settings->cacertLastDownloadedTimestamp = std::chrono::system_clock::now();
			settings->save();
		}
	}

	httpService->checkForInternetConnectivity();

	if(!notifyInitializationProgress("Initializing Time Zone Data Manager")) {
		m_initializing = false;
		return false;
	}

	TimeZoneDataManager * timeZoneDataManager = TimeZoneDataManager::getInstance();

	if(timeZoneDataManager->isSupported()) {
		bool timeZoneDataUpdated = false;
		bool shouldUpdateTimeZoneData = !settings->downloadThrottlingEnabled || !settings->timeZoneDataLastDownloadedTimestamp.has_value() || std::chrono::system_clock::now() - settings->timeZoneDataLastDownloadedTimestamp.value() > settings->timeZoneDataUpdateFrequency;

		if(!TimeZoneDataManager::getInstance()->initialize(Utilities::joinPaths(settings->dataDirectoryPath, settings->timeZoneDataDirectoryName), settings->fileETags, shouldUpdateTimeZoneData, false, &timeZoneDataUpdated)) {
			spdlog::error("Failed to initialize time zone data manager!");
		}

		if(timeZoneDataUpdated) {
			settings->timeZoneDataLastDownloadedTimestamp = std::chrono::system_clock::now();
			settings->save();
		}
	}

	std::chrono::milliseconds initializationDuration(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - initializeSteadyStartTimePoint));

	spdlog::info("{} initialized successfully after {} milliseconds!", APPLICATION_NAME, initializationDuration.count());

	m_initialized = true;
	m_initializing = false;

	if(!handleArguments(m_arguments.get())) {
		m_argumentHandlingFailed = true;
	}

	notifyInitializationProgress("Initialization Complete");

	return true;
}

void DukeForge::uninitialize() {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	if(!m_initialized) {
		return;
	}

	SettingsManager * settings = SettingsManager::getInstance();
	settings->save(m_arguments.get());

	m_arguments.reset();

	m_initialized = false;
}

bool DukeForge::abortInitialization() {
	if(!m_initializing) {
		return false;
	}

	spdlog::info("Aborting initialization.");

	m_initializationAborted = true;

	return true;
}

bool DukeForge::didArgumentHandlingFail() const {
	return m_argumentHandlingFailed;
}

bool DukeForge::handleArguments(const ArgumentParser * args) {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	if(args == nullptr) {
		return true;
	}

	return true;
}

std::string DukeForge::getArgumentHelpInformation() {
	std::ostringstream argumentHelpStream;

	argumentHelpStream << APPLICATION_NAME << " version " << APPLICATION_VERSION << " arguments:\n";
	argumentHelpStream << " --version - displays the application version.\n";
	argumentHelpStream << " --info - displays application and dependency library version information.\n";
	argumentHelpStream << " --help - displays this help message.\n";
	argumentHelpStream << " -? - alias for 'help'.\n";

	return argumentHelpStream.str();
}
