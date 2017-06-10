find_package (Qt5 REQUIRED COMPONENTS Core)

add_library (vatsinatordatabase MODULE
    dataprovider.h
    dataprovider.cpp

    data.qrc
    vatsinatordatabase.json
)

target_compile_definitions (vatsinatordatabase PRIVATE -DQT_PLUGIN -DQT_SHARED)
target_link_libraries (vatsinatordatabase vtrmisc)

vatsinator_add_plugin (vatsinatordatabase)

