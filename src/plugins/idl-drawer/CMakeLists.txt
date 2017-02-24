find_package (Qt5Core REQUIRED)
find_package (Qt5Network REQUIRED)
find_package (Qt5Gui REQUIRED)

add_library (idldrawer MODULE
    idldrawer.h
    idldrawer.cpp
    idldrawer.json
)

target_compile_definitions (idldrawer PRIVATE -DQT_PLUGIN -DQT_SHARED)
target_link_libraries (idldrawer vtrgui)

vatsinator_add_plugin (idldrawer)
