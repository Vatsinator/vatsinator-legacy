if (LINUX)
    set (vatsinator_PREFIX "${CMAKE_INSTALL_PREFIX}/share/vatsinator/")
elseif (APPLE OR WIN32 OR ANDROID)
    set (vatsinator_PREFIX "")
endif ()

set (vatsinator_MODULES
    core
    gui
    misc
    plugins
)


# set up platform-specific modules
if (WIN32 OR APPLE OR LINUX)
    set (BUILD_WIDGETS 1)
    list (APPEND vatsinator_MODULES widgets)
elseif (ANDROID)
    set (BUILD_QML 1)
    list (APPEND vatsinator_MODULES imports)
endif ()

# set up platform-specific installation paths
if (LINUX)
    # prefix-dependent, e.g. /usr/share/vatsinator/translations
    set (vatsinator_RESOURCES_TARGET_DIR "share/vatsinator")
    # prefix-dependent, e.g. /usr/share/bin/vatsinator
    set (vatsinator_BINARY_TARGET_DIR "bin")
    # prefix-dependent, e.g. /usr/share/lib/vatsinator/libvtrcore.so
    set (vatsinator_LIBRARY_TARGET_DIR "lib/vatsinator")
    # prefix-dependent, not used on Linux
    set (vatsinator_ARCHIVE_TARGET_DIR "lib")
elseif (WIN32)
    # location-independent, C:\Program Files\Vatsinator\translations
    set (vatsinator_RESOURCES_TARGET_DIR ".")
    # location-independent, C:\Program Files\Vatsinator\vatsinator.exe
    set (vatsinator_BINARY_TARGET_DIR ".")
    # location-independent, C:\Program Files\Vatsinator\vtrcore.dll
    set (vatsinator_LIBRARY_TARGET_DIR ".")
    # # location-independent, build/Debug/lib/vtrcore.lib
    set (vatsinator_ARCHIVE_TARGET_DIR "lib")
elseif (APPLE)
    # inside the bundle, e.g. vatsinator.app/Contents/Resources/translations
    set (vatsinator_RESOURCES_TARGET_DIR "vatsinator.app/Contents/Resources")
    # inside the bundle, e.g. vatsinator.app/Contents/MacOS/vatsinator
    set (vatsinator_BINARY_TARGET_DIR "vatsinator.app/Contents/MacOS")
    # linside the bundle, e.g. vatsinator.app/Contents/MacOS/libvtrcore.dylib
    set (vatsinator_LIBRARY_TARGET_DIR "vatsinator.app/Contents/MacOS")
    # not used on Linux
    set (vatsinator_ARCHIVE_TARGET_DIR "lib")
elseif (ANDROID)
    # in the assets, e.g. assets:/translations
    set (vatsinator_RESOURCES_TARGET_DIR "assets")
endif ()

foreach (module ${vatsinator_MODULES})
    list (APPEND vatsinator_SOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${module}")
endforeach ()

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTOUIC ON)
set (CMAKE_AUTORCC ON)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

include_directories (${CMAKE_CURRENT_SOURCE_DIR})
include_directories (${CMAKE_CURRENT_BINARY_DIR})

foreach (module ${vatsinator_MODULES})
    add_subdirectory (${module})
endforeach ()

add_subdirectory (i18n)
add_subdirectory (application)
