# 
#   Copyright (C) 2012-2015  Michał Garapich michal@garapich.pl
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

cmake_minimum_required (VERSION 3.0 FATAL_ERROR)

# Automatically link Qt executables to qtmain target on Windows
cmake_policy (SET CMP0020 NEW)

# Double colon in target name means ALIAS or IMPORTED target.
if (POLICY CMP0028)
    cmake_policy (SET CMP0028 NEW)
endif (POLICY CMP0028)

# Disallow use of the LOCATION target property.
if (POLICY CMP0026)
    cmake_policy (SET CMP0026 OLD)
endif (POLICY CMP0026)

if (UNIX AND NOT APPLE AND NOT MINGW AND NOT ANDROID)
    set (LINUX 1)
endif ()

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)


find_package (Qt5Core REQUIRED)
find_package (Qt5Gui REQUIRED)
find_package (Qt5Network REQUIRED)
find_package (QuaZip REQUIRED)

include_directories (${Qt5Core_INCLUDE_DIRS})
include_directories (${Qt5Gui_INCLUDE_DIRS})
include_directories (${Qt5Network_INCLUDE_DIRS})
include_directories (${QUAZIP_INCLUDE_DIRS})

if (ANDROID)
    find_package (Qt5Quick REQUIRED)
    include_directories(${Qt5Quick_INCLUDE_DIRS})
    
    find_package (Qt5AndroidExtras REQUIRED)
    include_directories (${Qt5AndroidExtras_INCLUDE_DIRS})
else () # desktop
    find_package (OpenGL REQUIRED)
    
    find_package (Qt5Widgets REQUIRED)
    include_directories (${Qt5Widgets_INCLUDE_DIRS})
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
    
    # TODO Qt 5.4: remove (QtOpenGLWidget)
    find_package (Qt5OpenGL REQUIRED)
    include_directories(${Qt5OpenGL_INCLUDE_DIRS})
endif()

if (APPLE)
    find_package (Qt5MacExtras REQUIRED)
    include_directories (${Qt5MacExtras_INCLUDE_DIRS})
endif ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
add_subdirectory (3rdparty)

include (QtUtils)

# set configuration types (msvc/xcode)
if (CMAKE_CONFIGURATION_TYPES)
  set (CMAKE_CONFIGURATION_TYPES Debug Release)
  set (CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
    "Reset the configurations to what we actually need"
    FORCE)
endif()

# set CMAKE_BUILD_TYPE (makefiles)
if (CMAKE_BUILD_TYPE MATCHES Debug)
  set (CMAKE_BUILD_TYPE Debug)
else ()
  set (CMAKE_BUILD_TYPE Release)
endif ()

# set CXX_FLAGS
if (CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  set (CMAKE_CXX_FLAGS_RELEASE "-DQT_NO_DEBUG -DQT_NO_DEBUG_OUTPUT")
  set (CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wpedantic -Wextra")
endif ()

# find version
if (NOT vatsinator_VERSION)
    file (READ ${CMAKE_CURRENT_SOURCE_DIR}/VERSION vatsinator_VERSION)
    string (REPLACE "\n" "" vatsinator_VERSION ${vatsinator_VERSION})
    
    include(GetGitRevisionDescription)
    get_git_head_revision(GIT_REFSPEC GIT_SHA1)
    
    if (NOT "${GIT_SHA1}" STREQUAL "GITDIR-NOTFOUND")
        # shorten the sha
        string (SUBSTRING "${GIT_SHA1}" 0 10 GIT_HASH)
        set (vatsinator_VERSION ${vatsinator_VERSION}-${GIT_HASH}-git)
    endif (NOT "${GIT_SHA1}" STREQUAL "GITDIR-NOTFOUND")
endif (NOT vatsinator_VERSION)

if (LINUX)
    set (vatsinator_INSTALL_DIR "share/vatsinator")
elseif (WIN32)
    set (vatsinator_INSTALL_DIR ".")
elseif (APPLE)
    set (vatsinator_INSTALL_DIR "vatsinator.app/Contents/Resources")
elseif (ANDROID)
    set (vatsinator_INSTALL_DIR "assets")
endif()

# specify user interface implementation
if (NOT vatsinator_UI_IMPLEMENTATION)
  if (WIN32 OR LINUX OR APPLE) # use QtWidgets for desktop
    set (vatsinator_UI_IMPLEMENTATION "widgets")
  elseif (ANDROID) # use QtQuick for mobile
    set (vatsinator_UI_IMPLEMENTATION "quick")
  endif ()
endif ()

if (NOT APPLE)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif (NOT APPLE)

add_subdirectory (source)

if (ANDROID)
    set (vatsinator_ANDROID_APK ${CMAKE_BINARY_DIR}/vatsinator.apk)
    set (CMAKE_INSTALL_PREFIX ${vatsinator_ANDROID_APK})
endif (ANDROID)

add_subdirectory (resources)

include (Packaging)

# Platform-specific actions
if (LINUX)
    # This sets up the "debian" directory
    option (WITH_DEBIAN "Setup \"debian\" subdirectory" OFF)
    if (WITH_DEBIAN AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/debian")
        add_subdirectory (debian)
    endif ()
    
    # Spec file
    option (WITH_RPM "Create RPM package" OFF)
endif ()
add_subdirectory (dist)

set (vatsinator_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)

# Sum up our configuration
message (STATUS "Vatsinator-${vatsinator_VERSION} will be built with the following options:")
message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")

if (NOT CMAKE_CONFIGURATION_TYPES)
  message (STATUS " * build type: ${CMAKE_BUILD_TYPE}")
else ()
  message (STATUS " * configuration types: ${CMAKE_CONFIGURATION_TYPES}")
endif ()

if (LINUX)
  message (STATUS " * platform: Linux")
elseif (APPLE)
  message (STATUS " * platform: Apple")
elseif (WIN32)
  message (STATUS " * platform: Win32")
elseif (ANDROID)
  message (STATUS " * platform: Android (${ANDROID_ABI})")
else ()
  message (STATUS " * platform: Undefined")
endif ()

query_qmake (QT_VERSION QT_VERSION)
message (STATUS " * Qt version: ${QT_VERSION}")

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

# doxygen documentation
find_package (Doxygen QUIET)
if (DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Generating documentation with Doxygen" VERBATIM
  )
endif ()

