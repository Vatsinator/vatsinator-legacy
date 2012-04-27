vatsinator_start_subdirectory (settings)

set (settings_SRCS
	SettingsManager.cpp
)

vatsinator_subdirectory_sources (settings "${settings_SRCS}")

set (settings_MOC_SRCS
	SettingsManager.h
)

vatsinator_subdirectory_mocs (settings "${settings_MOC_SRCS}")

vatsinator_end_subdirectory (settings)

add_dependencies(settings ui)

