set (vatsim_SOURCES
    modelfactory.cpp
    vatsimqmlplugin.cpp
)

set (vatsim_HEADERS
    modelfactory.h
    vatsimqmlplugin.h
)

add_library (vatsimplugin MODULE
    ${vatsim_SOURCES}
    ${vatsim_HEADERS}
)

target_link_libraries (vatsimplugin Qt5::Qml Qt5::Quick vtrcore vtrmisc)
vatsinator_add_qml_plugin (vatsimplugin org/eu/vatsinator/Vatsim)
