set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTOUIC ON)
set (CMAKE_AUTORCC ON)

include_directories (${CMAKE_SOURCE_DIR}/src)
include_directories (${CMAKE_BINARY_DIR}/src)

add_subdirectory (core)
