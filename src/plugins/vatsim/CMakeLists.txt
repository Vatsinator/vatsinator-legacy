find_package (Qt5 REQUIRED COMPONENTS Core Network)

add_library (vatsim MODULE
    servertracker.h
    servertracker.cpp

    vatsim.json
)

target_compile_definitions (vatsim PRIVATE -DQT_PLUGIN -DQT_SHARED)
target_link_libraries (vatsim vtrcore)

vatsinator_add_plugin (vatsim)
