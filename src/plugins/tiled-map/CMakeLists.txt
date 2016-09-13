find_package (Qt5Gui REQUIRED)
include_directories (${Qt5Gui_INCLUDE_DIRS}) 

add_library (tiledmapdrawer MODULE
    tiledmapdrawer.h
    tilemanager.h
    tile.h
    tilerenderer.h
    
    tiledmapdrawer.cpp
    tilemanager.cpp
    tile.cpp
    tilerenderer.cpp
    
    tiledmapdrawer.json
)

target_compile_definitions (tiledmapdrawer PRIVATE -DQT_PLUGIN -DQT_SHARED)
target_link_libraries (tiledmapdrawer Qt5::Gui vtrgui vtrmisc)

if (NOT ANDROID)
    install (TARGETS tiledmapdrawer DESTINATION ${vatsinator_RESOURCES_TARGET_DIR}/plugins)
endif ()
