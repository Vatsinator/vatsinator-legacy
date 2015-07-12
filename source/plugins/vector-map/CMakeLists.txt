qt5_add_resources (vectormapdrawer_RESOURCES vectormap.qrc)

add_library(vectormapdrawer STATIC
    vectormapdrawer.h
    worldmap.h
    
    vectormapdrawer.cpp
    worldmap.cpp
    
    ${vectormapdrawer_RESOURCES}
)

target_compile_definitions (vectormapdrawer PRIVATE -DQT_STATICPLUGIN -DQT_PLUGIN)

vatsinator_static_plugin (vectormapdrawer VectorMapDrawer)
