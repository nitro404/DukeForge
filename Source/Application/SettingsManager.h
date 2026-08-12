#ifndef _SETTINGS_MANAGER_H_
#define _SETTINGS_MANAGER_H_

#include <Dimension.h>
#include <Point2D.h>
#include <Singleton/Singleton.h>

#include <rapidjson/document.h>

#include <chrono>
#include <optional>
#include <string>

class ArgumentParser;

class SettingsManager final : public Singleton<SettingsManager> {
public:
	SettingsManager();
	~SettingsManager() override;

	void reset();

	rapidjson::Document toJSON() const;
	bool parseFrom(const rapidjson::Value & settingsDocument);

	bool isLoaded() const;
	bool load(const ArgumentParser * arguments = nullptr, bool autoCreate = true);
	bool save(bool overwrite = true) const;
	bool loadFrom(const std::string & filePath, bool autoCreate = true);
	bool saveTo(const std::string & filePath, bool overwrite = true) const;

	static const std::string FILE_TYPE;
	static const uint32_t FILE_FORMAT_VERSION;
	static const std::string DEFAULT_SETTINGS_FILE_PATH;
	static const std::string DEFAULT_DATA_DIRECTORY_PATH;
	static const std::string DEFAULT_TIME_ZONE_DATA_DIRECTORY_NAME;
	static const std::string DEFAULT_CURL_DATA_DIRECTORY_NAME;
	static const std::chrono::seconds DEFAULT_CONNECTION_TIMEOUT;
	static const std::chrono::seconds DEFAULT_NETWORK_TIMEOUT;
	static const std::chrono::seconds DEFAULT_TRANSFER_TIMEOUT;
	static const bool DEFAULT_VERBOSE_REQUEST_LOGGING;
	static const bool DEFAULT_SEGMENT_ANALYTICS_ENABLED;
	static const std::string DEFAULT_SEGMENT_ANALYTICS_DATA_FILE_NAME;
	static const bool DEFAULT_DOWNLOAD_THROTTLING_ENABLED;
	static const std::chrono::minutes DEFAULT_CACERT_UPDATE_FREQUENCY;
	static const std::chrono::minutes DEFAULT_TIME_ZONE_DATA_UPDATE_FREQUENCY;
	static const Point2D DEFAULT_WINDOW_POSITION;
	static const Dimension DEFAULT_WINDOW_SIZE;
	static const Dimension MINIMUM_WINDOW_SIZE;

	std::string dataDirectoryPath;
	std::string timeZoneDataDirectoryName;
	std::string curlDataDirectoryName;
	std::chrono::seconds connectionTimeout;
	std::chrono::seconds networkTimeout;
	std::chrono::seconds transferTimeout;
	bool verboseRequestLogging;
	bool segmentAnalyticsEnabled;
	std::string segmentAnalyticsDataFileName;
	bool downloadThrottlingEnabled;
	std::optional<std::chrono::time_point<std::chrono::system_clock>> cacertLastDownloadedTimestamp;
	std::chrono::minutes cacertUpdateFrequency;
	std::optional<std::chrono::time_point<std::chrono::system_clock>> timeZoneDataLastDownloadedTimestamp;
	std::chrono::minutes timeZoneDataUpdateFrequency;
	bool analyticsConfirmationAcknowledged;
	Point2D windowPosition;
	Dimension windowSize;

	std::map<std::string, std::string> fileETags;

private:
	bool m_loaded;
	std::string m_filePath;

	SettingsManager(const SettingsManager &) = delete;
	const SettingsManager & operator = (const SettingsManager &) = delete;
};

#endif // _SETTINGS_MANAGER_H_
