include_guard()

set(MAIN_SOURCE_FILES
	Application/DukeForge.h
	Application/DukeForge.cpp
	Application/SettingsManager.h
	Application/SettingsManager.cpp
	Game/NoCDCracker.h
	Game/NoCDCracker.cpp
	Main.cpp
	Project.h
)

list(APPEND SOURCE_FILES ${MAIN_SOURCE_FILES} ${MAIN_SOURCE_FILES_${PLATFORM_UPPER}})

list(TRANSFORM SOURCE_FILES PREPEND "${_SOURCE_DIRECTORY}/")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/${_SOURCE_DIRECTORY}" PREFIX "Source Files" FILES ${SOURCE_FILES})
