find_package (Qt5Core REQUIRED)
find_package (Qt5Network REQUIRED)
find_package (Qt5Gui REQUIRED)

add_library (travelpayoutsairlinelogoprovider MODULE
    travelpayoutsairlinelogoprovider.h
    travelpayoutsairlinelogoprovider.cpp
    travelpayoutsairlinelogoprovider.json
)

target_compile_definitions (travelpayoutsairlinelogoprovider PRIVATE -DQT_PLUGIN -DQT_SHARED)
target_link_libraries (travelpayoutsairlinelogoprovider vtrmisc)

if (NOT ANDROID)
    install (TARGETS travelpayoutsairlinelogoprovider DESTINATION ${vatsinator_RESOURCES_TARGET_DIR}/plugins)
endif ()
