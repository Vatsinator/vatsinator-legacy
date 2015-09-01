add_library (mapplugin MODULE
    map.cpp
    map.h
    mapqmlplugin.cpp
    mapqmlplugin.h
)

target_link_libraries (mapplugin Qt5::Qml vatsinator)
set_target_properties (mapplugin PROPERTIES
    PLUGIN_DIR qml/Map
    QMLDIR_FILE ${CMAKE_CURRENT_SOURCE_DIR}/qmldir
)
