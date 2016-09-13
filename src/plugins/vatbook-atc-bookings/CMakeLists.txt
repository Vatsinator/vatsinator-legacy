find_package (Qt5Core REQUIRED)

add_library (vatbookatcbookingprovider MODULE
    vatbookatcbookingprovider.h
    vatbookatcbookingprovider.cpp
    vatbookatcbookingprovider.json
)

target_compile_definitions (vatbookatcbookingprovider PRIVATE -DQT_PLUGIN -DQT_SHARED)
target_link_libraries (vatbookatcbookingprovider Qt5::Core vtrmisc)

if (NOT ANDROID)
    install (TARGETS vatbookatcbookingprovider DESTINATION ${vatsinator_RESOURCES_TARGET_DIR}/plugins)
endif ()
