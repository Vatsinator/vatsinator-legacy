set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

if (ANDROID)
    # http://stackoverflow.com/questions/26841603/arm-linux-androideabi-bin-ld-fatal-error-soname-must-take-a-non-empty-argume
    string(REPLACE "<CMAKE_SHARED_LIBRARY_SONAME_CXX_FLAG><TARGET_SONAME>" "" CMAKE_CXX_CREATE_SHARED_MODULE "${CMAKE_CXX_CREATE_SHARED_MODULE}")
endif ()

add_subdirectory (euroute-notams)
add_subdirectory (idl-drawer)
add_subdirectory (tiled-map)
add_subdirectory (travelpayouts-airline-logos)
add_subdirectory (vatbook-atc-bookings)
