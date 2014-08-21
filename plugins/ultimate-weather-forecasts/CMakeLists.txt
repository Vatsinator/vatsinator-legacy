if (${QT_VERSION} VERSION_LESS 5.0)
  qt4_wrap_cpp(uwf_MOCS ultimateweatherforecasts.h)
else (${QT_VERSION} VERSION_LESS 5.0)
  qt5_wrap_cpp(uwf_MOCS ultimateweatherforecasts.h)
  set (CMAKE_AUTOMOC ON)
  set (CMAKE_INCLUDE_CURRENT_DIR ON)
endif (${QT_VERSION} VERSION_LESS 5.0)
set (QT_USE_QTNETWORK TRUE)

add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

add_library(ultimateweatherforecasts SHARED ultimateweatherforecasts.cpp)

if (LINUX)
  install (TARGETS ultimateweatherforecasts DESTINATION share/vatsinator/plugins)
endif (LINUX)
