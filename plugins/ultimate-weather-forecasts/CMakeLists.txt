set (CMAKE_AUTOMOC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)

if (APPLE)
  set (CMAKE_MACOSX_RPATH ON)
endif ()

add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_NO_DEBUG)

add_library(ultimateweatherforecasts MODULE ultimateweatherforecasts.cpp)
# target_link_libraries(ultimateweatherforecasts vatsinatorsdk)

if (LINUX)
  install (TARGETS ultimateweatherforecasts DESTINATION share/vatsinator/plugins)
endif ()
