add_library(vectormapdrawer STATIC
    vectormapdrawer.h
    vectormapdrawer.cpp
)

target_compile_definitions (vectormapdrawer PRIVATE -DQT_STATICPLUGIN -DQT_PLUGIN)

vatsinator_static_plugin (vectormapdrawer VectorMapDrawer)
