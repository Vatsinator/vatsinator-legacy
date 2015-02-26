set (storage_SRCS
  abstractsettingsmodule.cpp
  cachefile.cpp
  filemanager.cpp
  languagemanager.cpp
  pluginmanager.cpp
  settingsmanager.cpp
  unzipper.cpp
)

set (storage_HEADERS
  abstractsettingsmodule.h
  cachefile.h
  filemanager.h
  languagemanager.h
  pluginmanager.h
  settingsmanager.h
  unzipper.h
)

add_library (storage OBJECT
  ${storage_SRCS}
  ${storage_HEADERS}
)

add_dependencies (storage ui)
