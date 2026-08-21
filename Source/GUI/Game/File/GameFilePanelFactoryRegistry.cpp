#include "GameFilePanelFactoryRegistry.h"

#include "Game/File/Group/GRP/GroupGRP.h"
#include "Game/File/Group/SSI/GroupSSI.h"
#include "Game/File/Palette/DAT/PaletteDAT.h"
#include "GUI/Game/File/Palette/DAT/PalettePanelDAT.h"
#include "GUI/Game/File/Group/GRP/GroupPanelGRP.h"
#include "GUI/Game/File/Group/SSI/GroupPanelSSI.h"
#include "GUI/WXUtilities.h"

#include <Utilities/FileUtilities.h>
#include <Utilities/StringUtilities.h>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <ranges>
#include <sstream>

static const std::string FILE_DIALOG_FILE_TYPES_SEPARATOR("|");

GameFilePanelFactoryRegistry::GameFilePanelFactoryRegistry()
	: m_defaultFactoriesAssigned(false) {
	assignDefaultFactories();
}

GameFilePanelFactoryRegistry::~GameFilePanelFactoryRegistry() { }

bool GameFilePanelFactoryRegistry::hasFactory(const std::string & fileNameOrExtension) const {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	return m_gameFilePanelFactories.find(GameFilePanelFactoryRegistry::formatFileNameOrExtension(fileNameOrExtension)) != m_gameFilePanelFactories.cend();
}

std::string GameFilePanelFactoryRegistry::getFactoryName(const std::string & fileNameOrExtension) const {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	GameFilePanelFactoryMap::const_iterator gameFilePanelFactoryIterator(m_gameFilePanelFactories.find(GameFilePanelFactoryRegistry::formatFileNameOrExtension(fileNameOrExtension)));

	if(gameFilePanelFactoryIterator == m_gameFilePanelFactories.cend()) {
		return {};
	}

	return gameFilePanelFactoryIterator->second.name;
}

std::vector<std::string> GameFilePanelFactoryRegistry::getFactoryFileExtensions() const {
	std::vector<std::string> factoryFileExtensions;
	factoryFileExtensions.reserve(factoryFileExtensions.size());

	for(const std::string & fileExtension : std::views::keys(m_gameFilePanelFactories)) {
		factoryFileExtensions.push_back(fileExtension);
	}

	return factoryFileExtensions;
}

bool GameFilePanelFactoryRegistry::setFactory(const std::string & fileNameOrExtension, const std::string & name, std::type_index gameFilePanelType, std::function<GameFilePanel *(std::unique_ptr<GameFile>, wxWindow *, wxWindowID, const wxPoint &, const wxSize &, long)> createNewGameFilePanelFunction) {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	if(fileNameOrExtension.empty() || createNewGameFilePanelFunction == nullptr) {
		return false;
	}

	std::string formattedFileNameOrExtension(formatFileNameOrExtension(fileNameOrExtension));

	if(formattedFileNameOrExtension.empty()) {
		return false;
	}

	m_gameFilePanelFactories.emplace(formattedFileNameOrExtension, GameFilePanelFactoryData({
		name,
		gameFilePanelType,
		createNewGameFilePanelFunction
	}));

	return true;
}

size_t GameFilePanelFactoryRegistry::setFactory(const std::vector<std::string> & fileNamesOrExtensions, const std::string & name, std::type_index gameFilePanelType, std::function<GameFilePanel *(std::unique_ptr<GameFile>, wxWindow *, wxWindowID, const wxPoint &, const wxSize &, long)> createNewGameFilePanelFunction) {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	size_t numberOfFactoriesSet = 0;

	for(const std::string & fileNameOrExtension : fileNamesOrExtensions) {
		if(setFactory(fileNameOrExtension, name, gameFilePanelType, createNewGameFilePanelFunction)) {
			numberOfFactoriesSet++;
		}
	}

	return numberOfFactoriesSet;
}

bool GameFilePanelFactoryRegistry::removeFactory(const std::string & fileNameOrExtension) {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	if(fileNameOrExtension.empty()) {
		return false;
	}

	std::string formattedFileNameOrExtension(formatFileNameOrExtension(fileNameOrExtension));

	if(formattedFileNameOrExtension.empty()) {
		return false;
	}

	GameFilePanelFactoryMap::const_iterator factoryDataIterator(m_gameFilePanelFactories.find(formattedFileNameOrExtension));

	if(factoryDataIterator == m_gameFilePanelFactories.cend()) {
		return false;
	}

	m_gameFilePanelFactories.erase(factoryDataIterator);

	return true;
}

void GameFilePanelFactoryRegistry::resetFactories() {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	m_gameFilePanelFactories.clear();
}

bool GameFilePanelFactoryRegistry::areDefaultFactoriesAssigned() const {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	return m_defaultFactoriesAssigned;
}

void GameFilePanelFactoryRegistry::assignDefaultFactories() {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	setFactory(PaletteDAT::FILE_FORMAT_EXTENSIONS, PaletteDAT::FILE_FORMAT_NAME, std::type_index(typeid(PalettePanelDAT)), [](std::unique_ptr<GameFile> gameFile, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style) {
		if(dynamic_cast<const PaletteDAT *>(gameFile.get()) == nullptr) {
			return static_cast<PalettePanelDAT *>(nullptr);
		}

		return new PalettePanelDAT(std::unique_ptr<PaletteDAT>(static_cast<PaletteDAT *>(gameFile.release())), parent, windowID, position, size, style);
	});

	setFactory(GroupGRP::FILE_FORMAT_EXTENSIONS, GroupGRP::FILE_FORMAT_NAME, std::type_index(typeid(GroupPanelGRP)), [](std::unique_ptr<GameFile> gameFile, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style) {
		if(dynamic_cast<const GroupGRP *>(gameFile.get()) == nullptr) {
			return static_cast<GroupPanelGRP *>(nullptr);
		}

		return new GroupPanelGRP(std::unique_ptr<GroupGRP>(static_cast<GroupGRP *>(gameFile.release())), parent, windowID, position, size, style);
	});

	setFactory(GroupSSI::FILE_FORMAT_EXTENSIONS, GroupSSI::FILE_FORMAT_NAME, std::type_index(typeid(GroupPanelSSI)), [](std::unique_ptr<GameFile> gameFile, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style) {
		if(dynamic_cast<const GroupSSI *>(gameFile.get()) == nullptr) {
			return static_cast<GroupPanelSSI *>(nullptr);
		}

		return new GroupPanelSSI(std::unique_ptr<GroupSSI>(static_cast<GroupSSI *>(gameFile.release())), parent, windowID, position, size, style);
	});
}

GameFilePanel * GameFilePanelFactoryRegistry::createNewGameFilePanel(std::unique_ptr<GameFile> gameFile, wxWindow * parent, wxWindowID windowID, const wxPoint & position, const wxSize & size, long style) {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	if(gameFile == nullptr) {
		return nullptr;
	}

	GameFilePanelFactoryMap::const_iterator gameFilePanelFactoryIterator(getGameFilePanelFactoryForFilePath(gameFile->getDefaultFileFormatExtension()));

	if(gameFilePanelFactoryIterator == m_gameFilePanelFactories.cend()) {
		spdlog::debug("Could not find game file panel factory for '{}'.");
		return nullptr;
	}

	return gameFilePanelFactoryIterator->second.createNewGameFilePanelFunction(std::move(gameFile), parent, windowID, position, size, style);
}

GameFilePanelFactoryRegistry::GameFilePanelFactoryMap::const_iterator GameFilePanelFactoryRegistry::getGameFilePanelFactoryForFilePath(const std::string & filePathOrExtension) const {
	std::lock_guard<std::recursive_mutex> lock(m_mutex);

	if(filePathOrExtension.empty()) {
		return m_gameFilePanelFactories.cend();
	}

	std::string formattedFileName(formatFileNameOrExtension(std::string(Utilities::getFileName(filePathOrExtension))));

	if(formattedFileName.empty()) {
		return m_gameFilePanelFactories.cend();
	}

	GameFilePanelFactoryMap::const_iterator gameFilePanelFactoryIterator(m_gameFilePanelFactories.find(formattedFileName));

	if(gameFilePanelFactoryIterator != m_gameFilePanelFactories.cend()) {
		return gameFilePanelFactoryIterator;
	}

	std::string formattedFileExtension(formatFileNameOrExtension(std::string(Utilities::getFileExtension(filePathOrExtension))));

	if(formattedFileExtension.empty()) {
		formattedFileExtension = filePathOrExtension;
	}

	gameFilePanelFactoryIterator = m_gameFilePanelFactories.find(formattedFileExtension);

	if(gameFilePanelFactoryIterator != m_gameFilePanelFactories.cend()) {
		return gameFilePanelFactoryIterator;
	}

	return std::find_if(m_gameFilePanelFactories.cbegin(), m_gameFilePanelFactories.cend(), [&filePathOrExtension](const auto & gameFileFactory) {
		return Utilities::endsWith(filePathOrExtension, gameFileFactory.first, false);
	});
}

std::string GameFilePanelFactoryRegistry::formatFileNameOrExtension(const std::string & fileNameOrExtension) {
	return Utilities::toLowerCase(Utilities::trimString(fileNameOrExtension));
}

std::vector<std::string> GameFilePanelFactoryRegistry::getAllFileFormatNames() const {
	std::vector<std::string> allFileFormatNames;

	for(GameFilePanelFactoryMap::const_iterator gameFilePanelFactoriesIterator = m_gameFilePanelFactories.cbegin(); gameFilePanelFactoriesIterator != m_gameFilePanelFactories.cend(); ++gameFilePanelFactoriesIterator) {
		allFileFormatNames.emplace_back(gameFilePanelFactoriesIterator->second.name);
	}

	return allFileFormatNames;
}

std::vector<std::string> GameFilePanelFactoryRegistry::getAllFileFormatFileExtensions() const {
	std::vector<std::string> allFileFormatFileExtensions;

	for(GameFilePanelFactoryMap::const_iterator gameFilePanelFactoriesIterator = m_gameFilePanelFactories.cbegin(); gameFilePanelFactoriesIterator != m_gameFilePanelFactories.cend(); ++gameFilePanelFactoriesIterator) {
		allFileFormatFileExtensions.emplace_back(gameFilePanelFactoriesIterator->first);
	}

	return allFileFormatFileExtensions;
}

std::vector<std::pair<std::string, std::string>> GameFilePanelFactoryRegistry::getAllFileFormatNamesAndExtensions() const {
	std::vector<std::pair<std::string, std::string>> allFileFormatNamesAndExtensions;

	for(GameFilePanelFactoryMap::const_iterator gameFilePanelFactoriesIterator = m_gameFilePanelFactories.cbegin(); gameFilePanelFactoriesIterator != m_gameFilePanelFactories.cend(); ++gameFilePanelFactoriesIterator) {
		allFileFormatNamesAndExtensions.emplace_back(gameFilePanelFactoriesIterator->second.name, gameFilePanelFactoriesIterator->first);
	}

	return allFileFormatNamesAndExtensions;
}

std::string GameFilePanelFactoryRegistry::getFileDialogFileType(const std::string & fileNameOrExtension) {
	GameFilePanelFactoryMap::const_iterator gameFilePanelFactoryIterator(getGameFilePanelFactoryForFilePath(fileNameOrExtension));

	if(gameFilePanelFactoryIterator == m_gameFilePanelFactories.cend()) {
		return {};
	}

	return getFileDialogFileType(gameFilePanelFactoryIterator->second.name, gameFilePanelFactoryIterator->first);
}

std::string GameFilePanelFactoryRegistry::getAllFileDialogFileTypesAsSeparateStrings(bool includeAllFilesEntry) {
	std::stringstream allFileDialogFileTypesStream;

	for(GameFilePanelFactoryMap::const_iterator gameFilePanelFactoriesIterator = m_gameFilePanelFactories.cbegin(); gameFilePanelFactoriesIterator != m_gameFilePanelFactories.cend(); ++gameFilePanelFactoriesIterator) {
		if(allFileDialogFileTypesStream.tellp() != 0) {
			allFileDialogFileTypesStream << FILE_DIALOG_FILE_TYPES_SEPARATOR;
		}

		allFileDialogFileTypesStream << getFileDialogFileType(gameFilePanelFactoriesIterator->second.name, gameFilePanelFactoriesIterator->first);
	}

	if(includeAllFilesEntry) {
		if(allFileDialogFileTypesStream.tellp() != 0) {
			allFileDialogFileTypesStream << FILE_DIALOG_FILE_TYPES_SEPARATOR;
		}

		allFileDialogFileTypesStream << WXUtilities::FILE_DIALOG_ALL_FILES;
	}

	return allFileDialogFileTypesStream.str();
}

std::string GameFilePanelFactoryRegistry::getFileDialogFileType(const std::string & name, const std::string fileExtension) {
	return fmt::format("{0} (*.{1}){2}*.{1}", name, fileExtension, FILE_DIALOG_FILE_TYPES_SEPARATOR);
}
