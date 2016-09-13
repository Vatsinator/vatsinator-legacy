set (components_SOURCES
    componentsqmlplugin.cpp
    flightprogress.cpp
    map.cpp
)

set (components_HEADERS
    componentsqmlplugin.h
    flightprogress.h
    map.h
)

set (components_FILES
    ClientListView.qml
    Details.qml
    MapControls.qml
    TwoLineListItem.qml
)

add_library (componentsplugin MODULE
    ${components_SOURCES}
    ${components_HEADERS}
    ${components_FILES}
    resources/icons.qrc
)

target_link_libraries (componentsplugin Qt5::Qml Qt5::Quick vtrgui)

vatsinator_add_qml_plugin (componentsplugin org/eu/vatsinator/Components
    FILES ${components_FILES}
)
