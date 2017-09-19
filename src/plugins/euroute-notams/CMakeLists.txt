find_package (Qt5Core REQUIRED)
find_package (Qt5Network REQUIRED)
find_package (Qt5Gui REQUIRED)

add_library (euroutenotamprovider MODULE
    euroutenotamprovider.h
    euroutenotamprovider.cpp
    euroutenotamprovider.json
)

target_compile_definitions (euroutenotamprovider PRIVATE -DQT_PLUGIN -DQT_SHARED)
target_link_libraries (euroutenotamprovider v8rmisc)

vatsinator_add_plugin (euroutenotamprovider)
