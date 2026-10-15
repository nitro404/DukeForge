#include "DukeForge.h"

#include "Game/File/GameFileFactoryRegistry.h"
#include "Game/File/Map/Map.h"
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

	const std::string MAP_DIRECTORY("C:\\Users\\nitro\\Desktop\\Duke Caribbean 2\\maps\\");

	const std::vector<std::string> RELATIVE_MAP_FILE_PATHS({
		"Cruise/E1M6_v7.map",
		"E1M1_Plane/E1M1_v31.map",
		"E1M2_Airport/E1M2_v35.map",
		"E1M3_Hotel/E1M3_v38.map",
		"E1M4_World/E1M4_v69.map",
		"E1M5_Winterland/E1M5_v44.map",
		"E1M6_Canaveral/E1M6_v46.map",
		"E1M7_Jurassic/E1M7_v66.map",
		"E1M8_Volcano/E1M8_v21.map"
	});

	const std::map<uint16_t, uint16_t> SPRITE_MAP({
		{ 2120, 2186 },
		{ 2121, 2187 },
		{ 2150, 2188 },
		{ 2165, 2189 },
		{ 1312, 1297 },
		{ 3400, 3393 }
	});

	for(const std::string &relativeMapFilePath : RELATIVE_MAP_FILE_PATHS) {
		const std::string mapFilePath(Utilities::joinPaths(MAP_DIRECTORY, relativeMapFilePath));

		std::unique_ptr<Map> map(Map::loadFrom(mapFilePath));

		if(map == nullptr) {
			spdlog::error("Failed to load map: '{}'.", mapFilePath);
			return false;
		}

		spdlog::info("Loaded map from file: '{}'.", relativeMapFilePath);

		for(size_t i = 0; i < map->numberOfSprites(); i++) {
			std::shared_ptr<Sprite> sprite = map->getSprite(i);
			std::map<uint16_t, uint16_t>::const_iterator spriteIterator(SPRITE_MAP.find(sprite->getTileNumber()));

			if(spriteIterator == SPRITE_MAP.cend()) {
				continue;
			}

			spdlog::info("Replacing sprite #{} tile #{} with #{}.", i + 1, spriteIterator->first, spriteIterator->second);

			sprite->setTileNumber(spriteIterator->second);
		}

		for(size_t i = 0; i < map->numberOfSprites(); i++) {
			std::shared_ptr<Sprite> sprite = map->getSprite(i);

			if(sprite->getTileNumber() != 9 || !sprite->hasHighTag()) {
				continue;
			}

			std::map<uint16_t, uint16_t>::const_iterator spriteIterator(SPRITE_MAP.find(sprite->getHighTag()));

			if(spriteIterator == SPRITE_MAP.cend()) {
				continue;
			}

			spdlog::info("Replacing respawn sprite #{} tile high tag #{} with #{}.", i + 1, spriteIterator->first, spriteIterator->second);

			sprite->setHighTag(spriteIterator->second);
		}

		std::string mapFileName(Utilities::getFileName(relativeMapFilePath));

		spdlog::info("Saving map '{}' to new file...", mapFileName);

		if(map->saveToMap(Utilities::joinPaths(MAP_DIRECTORY, mapFileName), false)) {
			spdlog::info("Successfully saved map '{}' to file!", mapFileName);
		}
		else {
			spdlog::error("Failed to save map '{}' to file!", mapFileName);
		}
	}

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
