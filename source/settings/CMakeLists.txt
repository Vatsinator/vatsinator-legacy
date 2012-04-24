set (settings_SRCS
	SettingsManager.cpp
)

set (settings_MOC_SRCS
	SettingsManager.h
)

vatsinator_subdirectory (settings "${settings_SRCS}" "${settings_MOC_SRCS}" "" "")

add_dependencies(settings ui)

