add_library (vatsimplugin MODULE
    vatsimqmlplugin.cpp
    vatsimqmlplugin.h
)

target_link_libraries (vatsimplugin Qt5::Qml vatsinator)
vatsinator_add_qml_plugin (vatsimplugin org/eu/vatsinator/Vatsim)
