set (CMAKE_AUTOMOC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)

add_library(ultimateweatherforecasts SHARED ultimateweatherforecasts.cpp)

if (LINUX)
  install (TARGETS ultimateweatherforecasts DESTINATION share/vatsinator/plugins)
endif (LINUX)
