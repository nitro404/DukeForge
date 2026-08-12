#include <Application/Application.h>
#include <Arguments/ArgumentParser.h>

#include <boost/signals2.hpp>

#include <atomic>
#include <cstdint>
#include <future>
#include <memory>
#include <mutex>

class DukeForge final : public Application {
public:
	DukeForge();
	~DukeForge() override;

	bool isInitialized() const;
	bool isInitializing() const;
	uint8_t numberOfInitializationSteps() const;
	bool initialize(int argc = 0, char * argv[] = nullptr, bool * aborted = nullptr);
	bool initialize(std::shared_ptr<ArgumentParser> arguments, bool * aborted = nullptr);
	void uninitialize();
	bool didArgumentHandlingFail() const;

	static std::string getArgumentHelpInformation();

	boost::signals2::signal<bool (uint8_t /* initializationStep */, uint8_t /* initializationStepCount */, std::string /* description */)> initializationProgress;

private:
	bool notifyInitializationProgress(const std::string & description, bool * aborted = nullptr);
	bool handleArguments(const ArgumentParser * args);

	std::atomic<bool> m_initialized;
	std::atomic<bool> m_initializing;
	std::atomic<uint8_t> m_initializationStep;
	std::atomic<bool> m_argumentHandlingFailed;
	std::shared_ptr<ArgumentParser> m_arguments;
	mutable std::recursive_mutex m_mutex;

	DukeForge(const DukeForge &) = delete;
	const DukeForge & operator = (const DukeForge &) = delete;
};
