#include "GameFileFactoryRegistry.h"

#include "Animation/ANM/AnimationANM.h"
#include "Art/Art.h"
#include "Group/GRP/GroupGRP.h"
#include "Group/SSI/GroupSSI.h"
#include "Map/Map.h"
#include "MIDI/MIDI.h"
#include "Palette/ACT/PaletteACT.h"
#include "Palette/CSS/PaletteCSS.h"
#include "Palette/DAT/PaletteDAT.h"
#include "Palette/GPL/PaletteGPL.h"
#include "Palette/JASC/PaletteJASC.h"
#include "Palette/KPL/PaletteKPL.h"
#include "Palette/PAL/PalettePAL.h"
#include "Palette/TXT/PaletteTXT.h"
#include "Sound/VOC/SoundVOC.h"
#include "Sound/WAV/SoundWAV.h"
#include "Zip/Zip.h"

#include <Utilities/FileUtilities.h>
#include <Utilities/StringUtilities.h>

#include <spdlog/spdlog.h>

#include <ranges>

GameFileFactoryRegistry::GameFileFactoryRegistry()
	: m_defaultFactoriesAssigned(false) {
	assignDefaultFactories();
}

GameFileFactoryRegistry::~GameFileFactoryRegistry() { }

bool GameFileFactoryRegistry::hasFactory(const std::string & fileNameOrExtension) const {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	return m_gameFileFactories.find(GameFileFactoryRegistry::formatFileNameOrExtension(fileNameOrExtension)) != m_gameFileFactories.cend();
}

std::string GameFileFactoryRegistry::getFactoryName(const std::string & fileNameOrExtension) const {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	GameFileFactoryMap::const_iterator gameFileFactoryIterator(m_gameFileFactories.find(GameFileFactoryRegistry::formatFileNameOrExtension(fileNameOrExtension)));

	if(gameFileFactoryIterator == m_gameFileFactories.cend()) {
		return {};
	}

	return gameFileFactoryIterator->second.name;
}

std::vector<std::string> GameFileFactoryRegistry::getFactoryFileExtensions() const {
	std::vector<std::string> factoryFileExtensions;
	factoryFileExtensions.reserve(factoryFileExtensions.size());

	for(const std::string & fileExtension : std::views::keys(m_gameFileFactories)) {
		factoryFileExtensions.push_back(fileExtension);
	}

	return factoryFileExtensions;
}

bool GameFileFactoryRegistry::setFactory(const std::string & fileNameOrExtension, const std::string & name, std::type_index gameFileType, std::function<std::unique_ptr<GameFile>()> createNewGameFileFunction, std::function<std::unique_ptr<GameFile>(const ByteBuffer & data)> readGameFileFunction, std::function<std::unique_ptr<GameFile>(const std::string & filePath)> loadGameFileFunction) {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	if(fileNameOrExtension.empty() || createNewGameFileFunction == nullptr || readGameFileFunction == nullptr || loadGameFileFunction == nullptr) {
		return false;
	}

	std::string formattedFileNameOrExtension(formatFileNameOrExtension(fileNameOrExtension));

	if(formattedFileNameOrExtension.empty()) {
		return false;
	}

	m_gameFileFactories.emplace(formattedFileNameOrExtension, GameFileFactoryData({
		name,
		gameFileType,
		createNewGameFileFunction,
		readGameFileFunction,
		loadGameFileFunction
	}));

	return true;
}

size_t GameFileFactoryRegistry::setFactory(const std::vector<std::string> & fileNamesOrExtensions, const std::string & name, std::type_index gameFileType, std::function<std::unique_ptr<GameFile>()> createNewGameFileFunction, std::function<std::unique_ptr<GameFile>(const ByteBuffer & data)> readGameFileFunction, std::function<std::unique_ptr<GameFile>(const std::string & filePath)> loadGameFileFunction) {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	size_t numberOfFactoriesSet = 0;

	for(const std::string & fileNameOrExtension : fileNamesOrExtensions) {
		if(setFactory(fileNameOrExtension, name, gameFileType, createNewGameFileFunction, readGameFileFunction, loadGameFileFunction)) {
			numberOfFactoriesSet++;
		}
	}

	return numberOfFactoriesSet;
}

bool GameFileFactoryRegistry::removeFactory(const std::string & fileNameOrExtension) {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	if(fileNameOrExtension.empty()) {
		return false;
	}

	std::string formattedFileNameOrExtension(formatFileNameOrExtension(fileNameOrExtension));

	if(formattedFileNameOrExtension.empty()) {
		return false;
	}

	GameFileFactoryMap::const_iterator factoryDataIterator(m_gameFileFactories.find(formattedFileNameOrExtension));

	if(factoryDataIterator == m_gameFileFactories.cend()) {
		return false;
	}

	m_gameFileFactories.erase(factoryDataIterator);

	return true;
}

void GameFileFactoryRegistry::resetFactories() {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	m_gameFileFactories.clear();
}

bool GameFileFactoryRegistry::areDefaultFactoriesAssigned() const {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	return m_defaultFactoriesAssigned;
}

void GameFileFactoryRegistry::assignDefaultFactories() {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	setFactory(PaletteACT::FILE_FORMAT_EXTENSIONS, PaletteACT::FILE_FORMAT_NAME, std::type_index(typeid(PaletteACT)), []() {
		return std::make_unique<PaletteACT>();
	}, [](const ByteBuffer & data) {
		return PaletteACT::readFrom(data);
	}, [](const std::string & filePath) {
		return PaletteACT::loadFrom(filePath);
	});

	setFactory(AnimationANM::FILE_FORMAT_EXTENSIONS, AnimationANM::FILE_FORMAT_NAME, std::type_index(typeid(AnimationANM)), []() {
		return std::make_unique<AnimationANM>();
	}, [](const ByteBuffer & data) {
		return AnimationANM::readFrom(data);
	}, [](const std::string & filePath) {
		return AnimationANM::loadFrom(filePath);
	});

	setFactory(Art::FILE_FORMAT_EXTENSIONS, Art::FILE_FORMAT_NAME, std::type_index(typeid(Art)), []() {
		return std::make_unique<Art>();
	}, [](const ByteBuffer & data) {
		return Art::readFrom(data);
	}, [](const std::string & filePath) {
		return Art::loadFrom(filePath);
	});

	setFactory(PaletteCSS::FILE_FORMAT_EXTENSIONS, PaletteCSS::FILE_FORMAT_NAME, std::type_index(typeid(PaletteCSS)), []() {
		return std::make_unique<PaletteCSS>();
	}, [](const ByteBuffer & data) {
		return PaletteCSS::readFrom(data);
	}, [](const std::string & filePath) {
		return PaletteCSS::loadFrom(filePath);
	});

	setFactory(PaletteDAT::FILE_FORMAT_EXTENSIONS, PaletteDAT::FILE_FORMAT_NAME, std::type_index(typeid(PaletteDAT)), []() {
		return std::make_unique<PaletteDAT>(*PaletteDAT::DEFAULT_PALETTE_DAT);
	}, [](const ByteBuffer & data) {
		return PaletteDAT::readFrom(data);
	}, [](const std::string & filePath) {
		return PaletteDAT::loadFrom(filePath);
	});

	setFactory(PaletteGPL::FILE_FORMAT_EXTENSIONS, PaletteGPL::FILE_FORMAT_NAME, std::type_index(typeid(PaletteGPL)), []() {
		return std::make_unique<PaletteGPL>();
	}, [](const ByteBuffer & data) {
		return PaletteGPL::readFrom(data);
	}, [](const std::string & filePath) {
		return PaletteGPL::loadFrom(filePath);
	});

	setFactory(GroupGRP::FILE_FORMAT_EXTENSIONS, GroupGRP::FILE_FORMAT_NAME, std::type_index(typeid(GroupGRP)), []() {
		return std::make_unique<GroupGRP>();
	}, [](const ByteBuffer & data) {
		return GroupGRP::readFrom(data);
	}, [](const std::string & filePath) {
		return GroupGRP::loadFrom(filePath);
	});

	setFactory(PaletteJASC::FILE_FORMAT_EXTENSIONS, PaletteJASC::FILE_FORMAT_NAME, std::type_index(typeid(PaletteJASC)), []() {
		return std::make_unique<PaletteJASC>();
	}, [](const ByteBuffer & data) {
		return PaletteJASC::readFrom(data);
	}, [](const std::string & filePath) {
		return PaletteJASC::loadFrom(filePath);
	});

	setFactory(PaletteKPL::FILE_FORMAT_EXTENSIONS, PaletteKPL::FILE_FORMAT_NAME, std::type_index(typeid(PaletteKPL)), []() {
		return std::make_unique<PaletteKPL>();
	}, [](const ByteBuffer & data) {
		return PaletteKPL::readFrom(data);
	}, [](const std::string & filePath) {
		return PaletteKPL::loadFrom(filePath);
	});

	setFactory(Map::FILE_FORMAT_EXTENSIONS, Map::FILE_FORMAT_NAME, std::type_index(typeid(Map)), []() {
		return std::make_unique<Map>();
	}, [](const ByteBuffer & data) {
		return Map::readFrom(data);
	}, [](const std::string & filePath) {
		return Map::loadFrom(filePath);
	});

	setFactory(MIDI::FILE_FORMAT_EXTENSIONS, MIDI::FILE_FORMAT_NAME, std::type_index(typeid(MIDI)), []() {
		return std::make_unique<MIDI>();
	}, [](const ByteBuffer & data) {
		return MIDI::readFrom(data);
	}, [](const std::string & filePath) {
		return MIDI::loadFrom(filePath);
	});

	setFactory(PalettePAL::FILE_FORMAT_EXTENSIONS, PalettePAL::FILE_FORMAT_NAME, std::type_index(typeid(PalettePAL)), []() {
		return std::make_unique<PalettePAL>();
	}, [](const ByteBuffer & data) {
		return PalettePAL::readFrom(data);
	}, [](const std::string & filePath) {
		return PalettePAL::loadFrom(filePath);
	});

	setFactory(GroupSSI::FILE_FORMAT_EXTENSIONS, GroupSSI::FILE_FORMAT_NAME, std::type_index(typeid(GroupSSI)), []() {
		return std::make_unique<GroupSSI>();
	}, [](const ByteBuffer & data) {
		return GroupSSI::readFrom(data);
	}, [](const std::string & filePath) {
		return GroupSSI::loadFrom(filePath);
	});

	setFactory(PaletteTXT::FILE_FORMAT_EXTENSIONS, PaletteTXT::FILE_FORMAT_NAME, std::type_index(typeid(PaletteTXT)), []() {
		return std::make_unique<PaletteTXT>();
	}, [](const ByteBuffer & data) {
		return PaletteTXT::readFrom(data);
	}, [](const std::string & filePath) {
		return PaletteTXT::loadFrom(filePath);
	});

	setFactory(SoundVOC::FILE_FORMAT_EXTENSIONS, SoundVOC::FILE_FORMAT_NAME, std::type_index(typeid(SoundVOC)), []() {
		return std::make_unique<SoundVOC>(SoundVOC::SubType::PCMUnsigned8Bit, 8000, 2);
	}, [](const ByteBuffer & data) {
		return SoundVOC::readFrom(data);
	}, [](const std::string & filePath) {
		return SoundVOC::loadFrom(filePath);
	});

	setFactory(SoundWAV::FILE_FORMAT_EXTENSIONS, SoundWAV::FILE_FORMAT_NAME, std::type_index(typeid(SoundWAV)), []() {
		return std::make_unique<SoundWAV>(SoundWAV::SubType::PCMUnsigned8Bit, 8000, 2);
	}, [](const ByteBuffer & data) {
		return SoundWAV::readFrom(data);
	}, [](const std::string & filePath) {
		return SoundWAV::loadFrom(filePath);
	});

	setFactory(Zip::FILE_FORMAT_EXTENSIONS, Zip::FILE_FORMAT_NAME, std::type_index(typeid(Zip)), []() {
		return std::make_unique<Zip>();
	}, [](const ByteBuffer & data) {
		return Zip::readFrom(data);
	}, [](const std::string & filePath) {
		return Zip::loadFrom(filePath);
	});
}

std::unique_ptr<GameFile> GameFileFactoryRegistry::createNewGameFile(const std::string & filePathOrExtension) {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	GameFileFactoryMap::const_iterator gameFileFactoryIterator(getGameFileFactoryForFilePath(filePathOrExtension));

	if(gameFileFactoryIterator == m_gameFileFactories.cend()) {
		spdlog::debug("Could not find game file factory for '{}'.");
		return nullptr;
	}

	return gameFileFactoryIterator->second.createNewGameFileFunction();
}

std::unique_ptr<GameFile> GameFileFactoryRegistry::readGameFileFrom(const ByteBuffer & data, const std::string & filePathOrExtension) {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	GameFileFactoryMap::const_iterator gameFileFactoryIterator(getGameFileFactoryForFilePath(filePathOrExtension));

	if(gameFileFactoryIterator == m_gameFileFactories.cend()) {
		spdlog::debug("Could not find game file factory for '{}'.");
		return nullptr;
	}

	return gameFileFactoryIterator->second.readGameFileFunction(data);
}

std::unique_ptr<GameFile> GameFileFactoryRegistry::loadGameFileFrom(const std::string & filePath) {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	GameFileFactoryMap::const_iterator gameFileFactoryIterator(getGameFileFactoryForFilePath(filePath));

	if(gameFileFactoryIterator == m_gameFileFactories.cend()) {
		spdlog::debug("Could not find game file factory for '{}'.");
		return nullptr;
	}

	return gameFileFactoryIterator->second.loadGameFileFunction(filePath);
}

GameFileFactoryRegistry::GameFileFactoryMap::const_iterator GameFileFactoryRegistry::getGameFileFactoryForFilePath(const std::string & filePathOrExtension) const {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	if(filePathOrExtension.empty()) {
		return m_gameFileFactories.cend();
	}

	std::string formattedFileName(formatFileNameOrExtension(std::string(Utilities::getFileName(filePathOrExtension))));

	if(formattedFileName.empty()) {
		return m_gameFileFactories.cend();
	}

	GameFileFactoryMap::const_iterator gameFileFactoryIterator(m_gameFileFactories.find(formattedFileName));

	if(gameFileFactoryIterator != m_gameFileFactories.cend()) {
		return gameFileFactoryIterator;
	}

	std::string formattedFileExtension(formatFileNameOrExtension(std::string(Utilities::getFileExtension(filePathOrExtension))));

	if(formattedFileExtension.empty()) {
		formattedFileExtension = filePathOrExtension;
	}

	gameFileFactoryIterator = m_gameFileFactories.find(formattedFileExtension);

	if(gameFileFactoryIterator != m_gameFileFactories.cend()) {
		return gameFileFactoryIterator;
	}

	return std::find_if(m_gameFileFactories.cbegin(), m_gameFileFactories.cend(), [&filePathOrExtension](const auto & gameFileFactory) {
		return Utilities::endsWith(filePathOrExtension, gameFileFactory.first, false);
	});
}

std::string GameFileFactoryRegistry::formatFileNameOrExtension(const std::string & fileNameOrExtension) {
	return Utilities::toLowerCase(Utilities::trimString(fileNameOrExtension));
}
