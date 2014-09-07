vatsinator_start_subdirectory (storage)

set (storage_SRCS
    abstractsettingsmodule.cpp
    cachefile.cpp
    filemanager.cpp
    languagemanager.cpp
    settingsmanager.cpp
    unzipper.cpp
)

vatsinator_subdirectory_sources (storage "${storage_SRCS}")

vatsinator_end_subdirectory (storage)


add_dependencies (storage ui)
