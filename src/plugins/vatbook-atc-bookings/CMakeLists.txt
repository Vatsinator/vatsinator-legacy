find_package (Qt5Core REQUIRED)
find_package (Qt5Network REQUIRED)
find_package (Qt5Gui REQUIRED)

add_library (vatbookatcbookingprovider MODULE
    vatbookatcbookingprovider.h
    vatbookatcbookingprovider.cpp
    vatbookatcbookingprovider.json
)

target_compile_definitions (vatbookatcbookingprovider PRIVATE -DQT_PLUGIN -DQT_SHARED)
target_link_libraries (vatbookatcbookingprovider vtrmisc)

if (NOT ANDROID)
    install (TARGETS vatbookatcbookingprovider DESTINATION ${vatsinator_RESOURCES_TARGET_DIR}/plugins)
endif ()
