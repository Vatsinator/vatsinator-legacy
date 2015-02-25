vatsinator_start_subdirectory (storage)

set (storage_SRCS
  abstractsettingsmodule.cpp
  cachefile.cpp
  filemanager.cpp
  languagemanager.cpp
  pluginmanager.cpp
  settingsmanager.cpp
  unzipper.cpp
)
vatsinator_subdirectory_sources (storage "${storage_SRCS}")

set (storage_HEADERS
  abstractsettingsmodule.h
  cachefile.h
  filemanager.h
  languagemanager.h
  pluginmanager.h
  settingsmanager.h
  unzipper.h
)
vatsinator_subdirectory_headers (storage "${storage_HEADERS}")

vatsinator_end_subdirectory (storage)


add_dependencies (storage ui)
