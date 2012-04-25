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

# find version
if (NOT vatsinator_VERSION)
	file (READ ${CMAKE_CURRENT_SOURCE_DIR}/VERSION vatsinator_VERSION)
	string (REPLACE "\n" "" vatsinator_VERSION ${vatsinator_VERSION})
endif (NOT vatsinator_VERSION)

add_subdirectory (source)

message (STATUS "Vatsinator-${vatsinator_VERSION} will be built with the following options:")
message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")
message (STATUS " * build type: ${CMAKE_BUILD_TYPE}")

# only for Linux
if (UNIX AND NOT APPLE)
	
	# create desktop entry file
	if (NOT DESKTOP_ENTRY)
		set (DESKTOP_ENTRY ${CMAKE_SOURCE_DIR}/vatsinator.desktop)
	endif (NOT DESKTOP_ENTRY)
	
	set (DESKTOP_SH_FILE ${CMAKE_SOURCE_DIR}/resources/vatsinator-desktop.sh)
	
	add_custom_command (OUTPUT ${DESKTOP_ENTRY}
		COMMAND touch ${DESKTOP_ENTRY}
		COMMAND sh ${DESKTOP_SH_FILE} ${CMAKE_INSTALL_PREFIX} > ${DESKTOP_ENTRY}
		DEPENDS ${DESKTOP_SH_FILE}
		COMMENT "Generating desktop entry file"
	)
	add_custom_target (DesktopEntryFile ALL
		DEPENDS ${DESKTOP_ENTRY}
	)
	
	# install files
	install (FILES
		resources/vatsinator.dat
		resources/WorldAirports.db
		resources/WorldFirs.db
		DESTINATION share/vatsinator
	)
	
	install (FILES ${DESKTOP_ENTRY}
		DESTINATION share/applications
	)
	
	macro (install_icon SIZE)
		install (FILES resources/vatsinator-${SIZE}.png
			DESTINATION "share/icons/hicolor/${SIZE}x${SIZE}/apps"
			RENAME vatsinator.png
		)
	endmacro (install_icon)
	
	set (SIZES 16 22 24 32 48 128)
	#install icons
	foreach (ICON ${SIZES})
		install_icon(${ICON})
	endforeach (ICON)
	
endif (UNIX AND NOT APPLE)


