add_library (tiledmapdrawer STATIC
    tile.h
    tiledmapdrawer.h
    tilemanager.h
    tileprovider.h
    tilereadyevent.h
    tileurl.h
    
    tile.cpp
    tiledmapdrawer.cpp
    tilemanager.cpp
    tileprovider.cpp
    tilereadyevent.cpp
    tileurl.cpp
)

target_compile_definitions (tiledmapdrawer PRIVATE -DQT_STATICPLUGIN -DQT_PLUGIN)

vatsinator_static_plugin (tiledmapdrawer TiledMapDrawer)
