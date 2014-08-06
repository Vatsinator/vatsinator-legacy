vatsinator_start_subdirectory (storage)

set (storage_SRCS
    abstractsettingspage.cpp
    cachefile.cpp
    filemanager.cpp
    languagemanager.cpp
    pluginmanager.cpp
    settingsmanager.cpp
    unzipper.cpp
)

vatsinator_subdirectory_sources (storage "${storage_SRCS}")

set (storage_MOC_SRCS
    cachefile.h
    pluginmanager.h
    settingsmanager.h
    unzipper.h
)

vatsinator_subdirectory_mocs (storage "${storage_MOC_SRCS}")

vatsinator_end_subdirectory (storage)


add_dependencies (storage ui)
