add_library (vatsimplugin MODULE
    vatsimqmlplugin.cpp
    vatsimqmlplugin.h
)

target_link_libraries (vatsimplugin Qt5::Qml vatsinator)
set_target_properties (vatsimplugin PROPERTIES
    PLUGIN_DIR qml/Vatsim
    QMLDIR_FILE ${CMAKE_CURRENT_SOURCE_DIR}/qmldir
)
