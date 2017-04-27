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

vatsinator_add_plugin (travelpayoutsairlinelogoprovider)
