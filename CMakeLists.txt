# 
# Copyright (C) 2012  Michał Garapich michal@garapich.pl
# 

project (vatsinator)

cmake_minimum_required (VERSION 2.8.0 FATAL_ERROR)

# find necessary libs
find_package (Qt4 4.7 COMPONENTS QtCore QtGui QtOpenGL QtXml QtSvg QtNetwork REQUIRED)
find_package (OpenGL REQUIRED)

if (UNIX AND NOT APPLE AND NOT MINGW)
    set (LINUX 1)
endif (UNIX AND NOT APPLE AND NOT MINGW)

if (MINGW)
    set (WIN32 1)
    
    # Cmake adds -rdynamic to compiler flags.
    # As mingw doesn't know rdynamic, we want to remove this.
    string (REPLACE "-rdynamic" "" CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS ${CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS})
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

# set windres command
if (MINGW)
  set (WINDRES_COMMAND i686-pc-mingw32-windres)
endif (MINGW)

# find version
if (NOT vatsinator_VERSION)
    file (READ ${CMAKE_CURRENT_SOURCE_DIR}/VERSION vatsinator_VERSION)
    string (REPLACE "\n" "" vatsinator_VERSION ${vatsinator_VERSION})
endif (NOT vatsinator_VERSION)

if (LINUX)
    # install files
    install (FILES
        COPYING
        resources/models.dat
        resources/WorldAirports.db
        resources/WorldFirs.db
        resources/WorldMap.db
        DESTINATION share/vatsinator
    )
    
    install (FILES
        resources/pixmaps/1p32.png
        resources/pixmaps/2p32.png
        resources/pixmaps/4p32.png
        resources/pixmaps/2j32.png
        resources/pixmaps/3j32.png
        resources/pixmaps/4j32.png
        resources/pixmaps/conc32.png
        DESTINATION share/vatsinator/pixmaps
    )
    
    install (FILES
        resources/data/alias
        resources/data/country
        resources/data/fir
        resources/data/uir
        DESTINATION share/vatsinator/data
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
    
    include (Packaging-linux)
    
    option (WITH_DEBIAN OFF)
    if (WITH_DEBIAN AND EXISTS "debian/")
        add_subdirectory (debian)
    endif (WITH_DEBIAN AND EXISTS "debian/")

    option (WITH_RPM OFF)
    
    add_subdirectory (dist)

elseif (APPLE)
    
    include (Packaging-darwin)
    
    set (vatsinator_BUNDLE_RESOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/COPYING
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/models.dat
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/WorldAirports.db
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/WorldFirs.db
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/WorldMap.db
    )
    
    set (vatsinator_BUNDLE_PIXMAPS
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/pixmaps/1p32.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/pixmaps/2p32.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/pixmaps/4p32.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/pixmaps/2j32.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/pixmaps/3j32.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/pixmaps/4j32.png
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/pixmaps/conc32.png
    )
    
    set (vatsinator_BUNDLE_DATA
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/data/alias
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/data/country
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/data/fir
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/data/uir
    )

# builds for win32
elseif (WIN32)
    
    if (MINGW AND NOT MINGW_LIBS_PATH)
        set (MINGW_LIBS_PATH ${CMAKE_CURRENT_BINARY_DIR})
    endif (MINGW AND NOT MINGW_LIBS_PATH)
    
    install (FILES
        ${QT_QTGUI_LIBRARY_RELEASE}
        ${QT_QTCORE_LIBRARY_RELEASE}
        ${QT_QTNETWORK_LIBRARY_RELEASE}
        ${QT_QTOPENGL_LIBRARY_RELEASE}
        ${QT_QTSVG_LIBRARY_RELEASE}
        ${QT_QTXML_LIBRARY_RELEASE}
        ${MINGW_LIBS_PATH}/libgcc_s_dw2-1.dll
        ${MINGW_LIBS_PATH}/mingwm10.dll
        DESTINATION .
    )
    
    install (FILES
        COPYING
        resources/models.dat
        resources/WorldAirports.db
        resources/WorldFirs.db
        resources/WorldMap.db
        DESTINATION .
    )
    
    install (FILES
        resources/pixmaps/1p32.png
        resources/pixmaps/2p32.png
        resources/pixmaps/4p32.png
        resources/pixmaps/2j32.png
        resources/pixmaps/3j32.png
        resources/pixmaps/4j32.png
        resources/pixmaps/conc32.png
        DESTINATION ./pixmaps
    )
    
     install (FILES
        resources/data/alias
        resources/data/country
        resources/data/fir
        resources/data/uir
        DESTINATION ./data
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


