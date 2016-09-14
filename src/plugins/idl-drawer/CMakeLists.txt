find_package (Qt5Core REQUIRED)
find_package (Qt5Network REQUIRED)
find_package (Qt5Gui REQUIRED)

add_library (idldrawer MODULE
    idldrawer.h
    idldrawer.cpp
    idldrawer.json
)

target_compile_definitions (idldrawer PRIVATE -DQT_PLUGIN -DQT_SHARED)
target_link_libraries (idldrawer vtrgui)

if (NOT ANDROID)
    install (TARGETS idldrawer DESTINATION ${vatsinator_RESOURCES_TARGET_DIR}/plugins)
endif ()

