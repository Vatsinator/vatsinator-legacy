qt4_wrap_cpp(uwf_MOCS ultimateweatherforecasts.h)
set (QT_USE_QTNETWORK TRUE)

add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)
add_definitions(-DQT_SHARED)

add_library(ultimateweatherforecasts SHARED ultimateweatherforecasts.cpp ${uwf_MOCS})
if (LINUX)
  install (TARGETS ultimateweatherforecasts DESTINATION share/vatsinator/plugins)
endif (LINUX)
