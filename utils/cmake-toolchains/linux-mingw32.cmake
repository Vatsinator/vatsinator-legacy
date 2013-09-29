# Copyright (C) 2012  Michał Garapich michal@garapich.pl
# 
# Toolchain to set up mingw32 environment under Linux.
#
# Usage:
#  cmake <path> -DCMAKE_TOOLCHAIN_FILE=<path>/utils/cmake-toolchains/linux-mingw32.cmake
# 
# There are three values you'll propably need to change if you want to build
# windows binaries under your Linux machine. These are:
# COMPILER_PREFIX - how your compiler environment can be recognized;
# QT_WIN32_PREFIX - where Qt libs, headers, bins, etc etc are installed;
# QT_X11_DIR - this is actually your QT binaries prefix. /usr/bin should be ok in most cases.
#

# the name of the target operating system
set (CMAKE_SYSTEM_NAME Windows)
set (WIN32 1)

# changing these three values should be enough
set (COMPILER_PREFIX "i686-pc-mingw32")
set (QT_WIN32_PREFIX /usr/${COMPILER_PREFIX}/usr/Qt)
set (QT_X11_DIR /usr/bin)
 
# which compilers to use for C and C++
set (CMAKE_RC_COMPILER   /usr/bin/${COMPILER_PREFIX}-windres)
set (CMAKE_C_COMPILER    /usr/bin/${COMPILER_PREFIX}-gcc)
set (CMAKE_CXX_COMPILER  /usr/bin/${COMPILER_PREFIX}-g++)

set (CMAKE_C_FLAGS "-static -static-libgcc" CACHE STRING "" FORCE)
set (CMAKE_CXX_FLAGS "-static -static-libgcc -static-libstdc++" CACHE STRING "" FORCE)
set (CMAKE_SHARED_LINKER_FLAGS "-static" CACHE STRING "" FORCE)
 
# here is the target environment located
set (CMAKE_FIND_ROOT_PATH
    /usr/${COMPILER_PREFIX}
    ${QT_WIN32_PREFIX}
)
 
# Adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, Search
# programs in the host environment
set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
 
# FindQt4.cmake querys qmake to get information, this doesn't work when crosscompiling
set (QT_BINARY_DIR   ${QT_WIN32_PREFIX}/bin)
set (QT_LIBRARY_DIR  ${QT_WIN32_PREFIX}/bin)
set (QT_INCLUDE_DIR  ${QT_WIN32_PREFIX}/include)
set (QT_MKSPECS_DIR  ${QT_WIN32_PREFIX}/mkspecs)

set (QT_MOC_EXECUTABLE      ${QT_X11_DIR}/moc)
set (QT_UIC_EXECUTABLE      ${QT_X11_DIR}/uic)
set (QT_RCC_EXECUTABLE      ${QT_X11_DIR}/rcc)
set (QT_QMAKE_EXECUTABLE    ${QT_X11_DIR}/qmake)
set (QT_LUPDATE_EXECUTABLE  ${QT_X11_DIR}/lupdate)
set (QT_LRELEASE_EXECUTABLE ${QT_X11_DIR}/lrelease)

set (QT_QTCORE_LIBRARY             ${QT_LIBRARY_DIR}/QtCore4.dll)
set (QT_QTCORE_INCLUDE_DIR         ${QT_INCLUDE_DIR}/QtCore)

set (QT_QTGUI_LIBRARY              ${QT_LIBRARY_DIR}/QtGui4.dll)
set (QT_QTGUI_INCLUDE_DIR          ${QT_INCLUDE_DIR}/QtGui)

set (QT_QTOPENGL_LIBRARY           ${QT_LIBRARY_DIR}/QtOpenGL4.dll)
set (QT_QTOPENGL_INCLUDE_DIR       ${QT_INCLUDE_DIR}/QtOpenGL)

set (QT_QTXML_LIBRARY              ${QT_LIBRARY_DIR}/QtXml4.dll)
set (QT_QTXML_INCLUDE_DIR          ${QT_INCLUDE_DIR}/QtXml)

set (QT_QTSVG_LIBRARY              ${QT_LIBRARY_DIR}/QtSvg4.dll)
set (QT_QTSVG_INCLUDE_DIR          ${QT_INCLUDE_DIR}/QtSvg)

set (QT_QTNETWORK_LIBRARY          ${QT_LIBRARY_DIR}/QtNetwork4.dll)
set (QT_QTNETWORK_INCLUDE_DIR      ${QT_INCLUDE_DIR}/QtNetwork)
