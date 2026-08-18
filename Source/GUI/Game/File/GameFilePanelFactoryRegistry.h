#ifndef _GAME_FILE_PANEL_FACTORY_REGISTRY_H_
#define _GAME_FILE_PANEL_FACTORY_REGISTRY_H_

#include "GameFilePanel.h"

#include <ByteBuffer.h>
#include <Singleton/Singleton.h>

#include <functional>
#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

class GameFilePanelFactoryRegistry final : public Singleton<GameFilePanelFactoryRegistry> {
public:
	GameFilePanelFactoryRegistry();
	virtual ~GameFilePanelFactoryRegistry();

	bool hasFactory(const std::string & fileNameOrExtension) const;
	std::string getFactoryName(const std::string & fileNameOrExtension) const;
	std::vector<std::string> getFactoryFileExtensions() const;
	bool setFactory(const std::string & fileNameOrExtension, const std::string & name, std::type_index gameFileType, std::function<std::unique_ptr<GameFilePanel>(std::unique_ptr<GameFile>, wxWindow *, wxWindowID, const wxPoint &, const wxSize &, long)> createNewGameFilePanelFunction);
	size_t setFactory(const std::vector<std::string> & fileNamesOrExtensions, const std::string & name, std::type_index gameFileType, std::function<std::unique_ptr<GameFilePanel>(std::unique_ptr<GameFile>, wxWindow *, wxWindowID, const wxPoint &, const wxSize &, long)> createNewGameFilePanelFunction);
	bool removeFactory(const std::string & fileNameOrExtension);
	void resetFactories();
	bool areDefaultFactoriesAssigned() const;
	void assignDefaultFactories();
	std::unique_ptr<GameFilePanel> createNewGameFilePanel(std::unique_ptr<GameFile> gameFile, wxWindow * parent, wxWindowID windowID = wxID_ANY, const wxPoint & position = wxDefaultPosition, const wxSize & size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxNO_BORDER);
	std::vector<std::string> getAllFileFormatNames() const;
	std::vector<std::string> getAllFileFormatFileExtensions() const;
	std::vector<std::pair<std::string, std::string>> getAllFileFormatNamesAndExtensions() const;
	std::string getFileDialogFileType(const std::string & fileNameOrExtension);
	std::string getAllFileDialogFileTypesAsSeparateStrings(bool includeAllFilesEntry = true);

private:
	static std::string getFileDialogFileType(const std::string & name, const std::string fileExtension);

	struct GameFilePanelFactoryData {
		std::string name;
		std::type_index gameFileType;
		std::function<std::unique_ptr<GameFilePanel>(std::unique_ptr<GameFile>, wxWindow *, wxWindowID, const wxPoint &, const wxSize &, long)> createNewGameFilePanelFunction;
	};

	typedef std::map<std::string, GameFilePanelFactoryData> GameFilePanelFactoryMap;

	GameFilePanelFactoryMap::const_iterator getGameFilePanelFactoryForFilePath(const std::string & filePathOrExtension) const;

	static std::string formatFileNameOrExtension(const std::string & fileExtension);

	GameFilePanelFactoryMap m_gameFilePanelFactories;
	bool m_defaultFactoriesAssigned;
	mutable std::recursive_mutex m_mutex;

	GameFilePanelFactoryRegistry(const GameFilePanelFactoryRegistry &) = delete;
	const GameFilePanelFactoryRegistry & operator = (const GameFilePanelFactoryRegistry &) = delete;
};

#endif // _GAME_FILE_PANEL_FACTORY_REGISTRY_H_
