find_package (Qt5 REQUIRED COMPONENTS Core Network Gui)

add_library (vatbookatcbookingprovider MODULE
    vatbookatcbookingprovider.h
    vatbookatcbookingprovider.cpp
    vatbookatcbookingprovider.json
)

target_compile_definitions (vatbookatcbookingprovider PRIVATE -DQT_PLUGIN -DQT_SHARED)
target_link_libraries (vatbookatcbookingprovider v8rmisc)

vatsinator_add_plugin (vatbookatcbookingprovider)
