#include "DukeForge.h"

#include "Project.h"
#include "SettingsManager.h"

#include <Factory/FactoryRegistry.h>
#include <LibraryInformation.h>
#include <Network/HTTPService.h>
#include <Platform/TimeZoneDataManager.h>
#include <Utilities/FileUtilities.h>
#include <Utilities/StringUtilities.h>

#include <gcem.hpp>
#include <spdlog/spdlog.h>

#include <chrono>

DukeForge::DukeForge()
	: Application()
	, m_initialized(false)
	, m_initializing(false) {
	FactoryRegistry & factoryRegistry = FactoryRegistry::getInstance();

	factoryRegistry.setFactory<SettingsManager>([]() {
		return std::make_unique<SettingsManager>();
	});

	LibraryInformation * libraryInformation = LibraryInformation::getInstance();
	libraryInformation->addLibrary("CSS-Color-Parser", CSS_COLOR_PARSER_VERSION);
	libraryInformation->addLibrary("GCE-Math", fmt::format("{}.{}.{}", GCEM_VERSION_MAJOR, GCEM_VERSION_MINOR, GCEM_VERSION_PATCH));
}

DukeForge::~DukeForge() { }

bool DukeForge::isInitialized() const {
	return m_initialized;
}

bool DukeForge::isInitializing() const {
	return m_initializing;
}

bool DukeForge::initialize(int argc, char * argv[]) {
	std::shared_ptr<ArgumentParser> arguments;

	if(argc != 0) {
		arguments = std::make_shared<ArgumentParser>(argc, argv);
	}

	return initialize(arguments);
}

bool DukeForge::initialize(std::shared_ptr<ArgumentParser> arguments) {
	if(m_initialized || m_initializing) {
		return false;
	}

	std::chrono::time_point<std::chrono::steady_clock> initializeSteadyStartTimePoint(std::chrono::steady_clock::now());
	m_initializing = true;

	if(arguments != nullptr) {
		m_arguments = arguments;
	}

	SettingsManager * settings = SettingsManager::getInstance();

	if(!settings->isLoaded()) {
		settings->load(m_arguments.get());
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

	return true;
}

void DukeForge::uninitialize() {
	if(!m_initialized) {
		return;
	}

	SettingsManager * settings = SettingsManager::getInstance();
	settings->save(m_arguments.get());

	m_arguments.reset();

	m_initialized = false;
}
