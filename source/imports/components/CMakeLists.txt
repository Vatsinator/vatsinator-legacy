set (components_FILES
    MenuDrawer.qml
    NavigationDrawer.qml
    
    images/ic_drawer.png
)

add_custom_target (componentsplugin)
vatsinator_add_qml_plugin (componentsplugin org/eu/vatsinator/Components
    FILES ${components_FILES}
)
