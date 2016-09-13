find_package (Qt5Core REQUIRED)

add_library (euroutenotamprovider MODULE
    euroutenotamprovider.h
    euroutenotamprovider.cpp
    euroutenotamprovider.json
)

target_compile_definitions (euroutenotamprovider PRIVATE -DQT_PLUGIN -DQT_SHARED)
target_link_libraries (euroutenotamprovider Qt5::Core vtrmisc)

if (NOT ANDROID)
    install (TARGETS euroutenotamprovider DESTINATION ${vatsinator_RESOURCES_TARGET_DIR}/plugins)
endif ()
