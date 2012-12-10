# 
#   Copyright (C) 2012  Michał Garapich michal@garapich.pl
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

project (vatsinator)

cmake_minimum_required (VERSION 2.8.0 FATAL_ERROR)

# find necessary libs
find_package (Qt4 4.8 COMPONENTS QtCore QtGui QtOpenGL QtXml QtSvg QtNetwork REQUIRED)
find_package (OpenGL REQUIRED)

if (UNIX AND NOT APPLE AND NOT MINGW)
    set (LINUX 1)
endif (UNIX AND NOT APPLE AND NOT MINGW)

if (MINGW AND CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)
    set (WIN32 1)
    
    # Cmake adds -rdynamic to compiler flags.
    # As mingw doesn't know rdynamic, we want to remove this.
    string (REPLACE "-rdynamic" "" CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS ${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS})
endif (MINGW AND CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS)

if (MINGW)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
endif (MINGW)

include(${QT_USE_FILE})

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

# set CMAKE_BUILD_TYPE
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set (CMAKE_BUILD_TYPE Debug)
else (CMAKE_BUILD_TYPE MATCHES Debug)
    set (CMAKE_BUILD_TYPE Release)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

# set CXX_FLAGS
set (CMAKE_CXX_FLAGS_RELEASE "-std=c++0x -DNO_DEBUG")
set (CMAKE_CXX_FLAGS_DEBUG "-g -Wall -std=c++0x")

# find version
if (NOT vatsinator_VERSION)
    file (READ ${CMAKE_CURRENT_SOURCE_DIR}/VERSION vatsinator_VERSION)
    string (REPLACE "\n" "" vatsinator_VERSION ${vatsinator_VERSION})
endif (NOT vatsinator_VERSION)

include (VatsinatorInstall)

set (vatsinator_RESOURCES
    COPYING
    resources/WorldAirports.db
    resources/WorldFirs.db
    resources/WorldMap.db)

vatsinator_install (
    "${vatsinator_RESOURCES}" "")

set (vatsinator_RESOURCES_PIXMAPS
    resources/pixmaps/1p32.png
    resources/pixmaps/2p32.png
    resources/pixmaps/4p32.png
    resources/pixmaps/2j32.png
    resources/pixmaps/3j32.png
    resources/pixmaps/4j32.png
    resources/pixmaps/conc32.png)

vatsinator_install (
    "${vatsinator_RESOURCES_PIXMAPS}" "/pixmaps")

set (vatsinator_RESOURCES_DATA
    resources/data/alias
    resources/data/country
    resources/data/fir
    resources/data/model
    resources/data/uir)

vatsinator_install (
    "${vatsinator_RESOURCES_DATA}" "/data")

if (LINUX)
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
    
    include (Packaging-linux)
    
    option (WITH_DEBIAN OFF)
    if (WITH_DEBIAN AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/debian")
        add_subdirectory (debian)
    endif (WITH_DEBIAN AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/debian")

    option (WITH_RPM OFF)
    
    add_subdirectory (dist)

elseif (APPLE)
    
    include (Packaging-darwin)

# builds for win32
elseif (WIN32)

    install (FILES
        ${QT_QTGUI_LIBRARY_RELEASE}
        ${QT_QTCORE_LIBRARY_RELEASE}
        ${QT_QTNETWORK_LIBRARY_RELEASE}
        ${QT_QTOPENGL_LIBRARY_RELEASE}
        ${QT_QTSVG_LIBRARY_RELEASE}
        ${QT_QTXML_LIBRARY_RELEASE}
        ${QT_LIBRARY_DIR}/libgcc_s_dw2-1.dll
        ${QT_LIBRARY_DIR}/mingwm10.dll
        DESTINATION .
    )
    
    include (Packaging-win32)
    
endif (LINUX)

add_subdirectory (source)

message (STATUS "Vatsinator-${vatsinator_VERSION} will be built with the following options:")
message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")
message (STATUS " * build type: ${CMAKE_BUILD_TYPE}")
if (LINUX)
    message (STATUS " * platform: Linux")
elseif (APPLE)
    message (STATUS " * platform: Apple")
elseif (WIN32)
    message (STATUS " * platform: Win32")
else (LINUX)
    message (STATUS " * platform: Undefined")
endif (LINUX)


# uninstall target
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
)

add_custom_target (
    uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)


