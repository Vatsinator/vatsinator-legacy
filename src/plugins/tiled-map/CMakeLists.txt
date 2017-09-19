find_package (Qt5Core REQUIRED)
find_package (Qt5Network REQUIRED)
find_package (Qt5Gui REQUIRED)

add_library (tiledmapdrawer MODULE
    tiledmapdrawer.h
    tiledmapdrawerplugin.h
    tilemanager.h
    tile.h
    tilerenderer.h
    
    tiledmapdrawer.cpp
    tiledmapdrawerplugin.cpp
    tilemanager.cpp
    tile.cpp
    tilerenderer.cpp
    
    tiledmapdrawer.json
)

target_compile_definitions (tiledmapdrawer PRIVATE -DQT_PLUGIN -DQT_SHARED)
target_link_libraries (tiledmapdrawer v8rgui v8rmisc)

vatsinator_add_plugin (tiledmapdrawer)
