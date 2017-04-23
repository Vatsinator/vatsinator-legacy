set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

if (ANDROID)
    # http://stackoverflow.com/questions/26841603/arm-linux-androideabi-bin-ld-fatal-error-soname-must-take-a-non-empty-argume
    string(REPLACE "<CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG><TARGET_SONAME>" "" CMAKE_CXX_CREATE_SHARED_MODULE "${CMAKE_CXX_CREATE_SHARED_MODULE}")
endif ()

if (LINUX)
    set (vatsinator_PLUGINS_TARGET_DIR "share/vatsinator/plugins")
elseif (WIN32)
    set (vatsinator_PLUGINS_TARGET_DIR "plugins")
elseif (APPLE)
    set (vatsinator_PLUGINS_TARGET_DIR "vatsinator.app/Contents/PlugIns")
elseif (ANDROID)
    set (vatsinator_RESOURCES_TARGET_DIR "assets/plugins")
endif()

set (vatsinator_PLUGINS_TARGET_DIR ${vatsinator_PLUGINS_TARGET_DIR} PARENT_SCOPE)

function (vatsinator_add_plugin target)
    if (NOT ANDROID)
        install (TARGETS ${target} DESTINATION ${vatsinator_PLUGINS_TARGET_DIR})
    endif ()
endfunction ()

add_subdirectory (euroute-notams)
add_subdirectory (idl-drawer)
add_subdirectory (marble-map)
add_subdirectory (tiled-map)
add_subdirectory (travelpayouts-airline-logos)
add_subdirectory (vatbook-atc-bookings)
