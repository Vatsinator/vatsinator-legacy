add_library (mapplugin MODULE
    map.cpp
    map.h
    mapqmlplugin.cpp
    mapqmlplugin.h
)

target_link_libraries (mapplugin Qt5::Qml vatsinator)
vatsinator_add_qml_plugin (mapplugin Map)
