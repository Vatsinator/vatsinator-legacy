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

if (WIN32 OR APPLE OR LINUX)
    set (BUILD_WIDGETS 1)
    list (APPEND vatsinator_MODULES widgets)
elseif (ANDROID)
    set (BUILD_QML 1)
    list (APPEND vatsinator_MODULES imports)
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
