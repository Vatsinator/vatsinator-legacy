find_package (Qt5Core REQUIRED)
include (CMakeFindDependencyMacro)
find_package (Marble REQUIRED)
add_library (marblemapdrawer MODULE
    marblemapdrawer.cpp
    marblemapdrawerplugin.cpp

    marblemapdrawer.h
    marblemapdrawerplugin.h

    marblemapdrawer.json
)

target_compile_definitions (marblemapdrawer PRIVATE -DQT_PLUGIN -DQT_SHARED)
target_link_libraries (marblemapdrawer Marble)

vatsinator_add_plugin (marblemapdrawer)
