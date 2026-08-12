#include <Application/Application.h>
#include <Arguments/ArgumentParser.h>

#include <atomic>
#include <memory>

class DukeForge final : public Application {
public:
	DukeForge();
	~DukeForge() override;

	bool isInitialized() const;
	bool isInitializing() const;
	bool initialize(int argc = 0, char * argv[] = nullptr);
	bool initialize(std::shared_ptr<ArgumentParser> arguments);
	void uninitialize();

	static std::string getArgumentHelpInformation();
	static void displayArgumentHelp();
	static void displayVersion();
	static void displayLibraryInformation();

private:
	std::atomic<bool> m_initialized;
	std::atomic<bool> m_initializing;
	std::shared_ptr<ArgumentParser> m_arguments;

	DukeForge(const DukeForge &) = delete;
	const DukeForge & operator = (const DukeForge &) = delete;
};
