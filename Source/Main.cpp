#include "Application/DukeForge.h"

#include "Project.h"

#include <Application/ComponentRegistry.h>

#include <spdlog/spdlog.h>

#include <memory>

int main(int argc, char * argv[]) {
	ComponentRegistry::getInstance().registerGlobalComponents();

	std::shared_ptr<ArgumentParser> arguments(std::make_shared<ArgumentParser>(argc, argv));

	std::unique_ptr<DukeForge> dukeForge(std::make_unique<DukeForge>());

	if(!dukeForge->initialize(arguments)) {
		spdlog::error("{} initialization failed!", APPLICATION_NAME);
	}

	dukeForge->uninitialize();

	arguments.reset();
	dukeForge.reset();

	ComponentRegistry::getInstance().deleteAllGlobalComponents();
}
