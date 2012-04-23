project (vatsinator)

cmake_minimum_required (VERSION 2.8.0 FATAL_ERROR)

# find necessary libs
find_package (Qt4	REQUIRED)
find_package (OpenGL	REQUIRED)

# set CMAKE_BUILD_TYPE
if (CMAKE_BUILD_TYPE MATCHES Debug)
	set (CMAKE_BUILD_TYPE Debug)
else (CMAKE_BUILD_TYPE MATCHES Debug)
	set (CMAKE_BUILD_TYPE Release)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

# set CXX_FLAGS
set (CMAKE_CXX_FLAGS_RELEASE "-std=c++0x -DNO_DEBUG")
set (CMAKE_CXX_FLAGS_DEBUG "-g -Wall -std=c++0x -fpermissive")

# set windres command
set (WINDRES_COMMAND i686-pc-mingw32-windres)

add_subdirectory (source)

message (STATUS "Vatsinator will be built:")
message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")
message (STATUS " * build type: ${CMAKE_BUILD_TYPE}")


# only for Linux
install (FILES
	resources/vatsinator.dat
	resources/WorldAirports.db
	resources/WorldFirs.db
	DESTINATION share/vatsinator
)


