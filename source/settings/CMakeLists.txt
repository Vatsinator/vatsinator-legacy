vatsinator_start_subdirectory (settings)

set (settings_SRCS
	settingsmanager.cpp
)

vatsinator_subdirectory_sources (settings "${settings_SRCS}")

set (settings_MOC_SRCS
	settingsmanager.h
)

vatsinator_subdirectory_mocs (settings "${settings_MOC_SRCS}")

vatsinator_end_subdirectory (settings)

add_dependencies(settings ui)

