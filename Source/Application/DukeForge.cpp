#include "DukeForge.h"

#include "Project.h"

#include <spdlog/spdlog.h>

#include <chrono>

DukeForge::DukeForge()
	: Application()
	, m_initialized(false)
	, m_initializing(false) { }

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

	m_arguments.reset();

	m_initialized = false;
}
