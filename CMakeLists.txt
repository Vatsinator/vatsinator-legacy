project (vatsinator)
cmake_minimum_required (VERSION 3.7)

# Automatically link Qt executables to qtmain target on Windows
if (POLICY CMP0020)
    cmake_policy (SET CMP0020 NEW)
endif ()

# Double colon in target name means ALIAS or IMPORTED target.
if (POLICY CMP0028)
    cmake_policy (SET CMP0028 NEW)
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Android")
    set (ANDROID 1)
endif ()

if (UNIX AND NOT APPLE AND NOT MINGW AND NOT ANDROID)
    set (LINUX 1)
endif ()

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

# set configuration types (msvc/xcode)
if (CMAKE_CONFIGURATION_TYPES)
    set (CMAKE_CONFIGURATION_TYPES Debug Release)
    set (CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
        "Reset the configurations to what we actually need" FORCE
)
endif()

# set CMAKE_BUILD_TYPE (makefiles)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set (CMAKE_BUILD_TYPE Debug)
else ()
    set (CMAKE_BUILD_TYPE Release)
endif ()

# set CXX_FLAGS
if (CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
    set (CMAKE_CXX_FLAGS_RELEASE "-DQT_NO_DEBUG -DQT_NO_DEBUG_OUTPUT")
    set (CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wpedantic -Wextra")
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")
    endif ()
elseif (MSVC)
    set_property (GLOBAL PROPERTY USE_FOLDERS ON)

    set (CMAKE_CXX_FLAGS_DEBUG "/W4")
    set (CMAKE_CXX_FLAGS_RELEASE "/DQT_NO_DEBUG /DQT_NO_DEBUG_OUTPUT")

    set(CMAKE_CXX_FLAGS_RELEASE "/MD")
    set(CMAKE_CXX_FLAGS_DEBUG "/MDd")
endif ()

set (CMAKE_CXX_STANDARD "14")

# find version
if (NOT vatsinator_VERSION)
    file (READ ${CMAKE_CURRENT_SOURCE_DIR}/VERSION vatsinator_VERSION)
    string (REPLACE "\n" "" vatsinator_VERSION ${vatsinator_VERSION})
    
    include (GetGitRevisionDescription)
    get_git_head_revision(GIT_REFSPEC GIT_SHA1)
    
    if (NOT "${GIT_SHA1}" STREQUAL "GITDIR-NOTFOUND")
        # shorten the sha
        string (SUBSTRING "${GIT_SHA1}" 0 10 GIT_HASH)
        set (vatsinator_VERSION ${vatsinator_VERSION}-${GIT_HASH}-git)
    endif (NOT "${GIT_SHA1}" STREQUAL "GITDIR-NOTFOUND")
endif (NOT vatsinator_VERSION)

set (CMAKE_SKIP_BUILD_RPATH FALSE)
set (CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set (CMAKE_INSTALL_RPATH "")
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

if (APPLE AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "/Applications" CACHE PATH "..." FORCE)
endif ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)
add_subdirectory (3rdparty)

add_subdirectory (src)

find_package (Qt5Test QUIET)
if (Qt5Test_FOUND)
    include_directories (${Qt5Test_INCLUDE_DIRS})
    include (CTest)
    enable_testing ()
    add_subdirectory (tests)
endif ()

# Sum up our configuration
message (STATUS "Vatsinator-${vatsinator_VERSION} will be built with the following options:")
message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")

if (NOT CMAKE_CONFIGURATION_TYPES)
    message (STATUS " * build type: ${CMAKE_BUILD_TYPE}")
else ()
    message (STATUS " * configuration types: ${CMAKE_CONFIGURATION_TYPES}")
endif ()

if (ANDROID)
    message (STATUS " * platform: Android ABI: ${CMAKE_ANDROID_ARCH_ABI}")
endif ()

if (BUILD_TESTING)
    message (STATUS " * tests: enabled")
else ()
    message (STATUS " * tests: disabled")
endif ()

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
    include (QtUtils)
    query_qmake(qt_doc_root QT_INSTALL_DOCS)

    if (qt_doc_root)
        file (GLOB qt_doc_dirs RELATIVE ${qt_doc_root} ${qt_doc_root}/*)
        foreach (dir ${qt_doc_dirs})
            set (tag_file ${qt_doc_root}/${dir}/${dir}.tags)
            if (EXISTS ${tag_file})
                list (APPEND qt_tags "${tag_file}=http://doc.qt.io/qt-5/")
            endif ()
        endforeach ()

        string (REGEX REPLACE ";" " \\\\\n" QT_TAGS "${qt_tags}")
    endif ()

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating documentation with Doxygen" VERBATIM
    )
endif ()
