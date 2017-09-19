find_package (Qt5 5.7 REQUIRED COMPONENTS Core Network)

set (core_SOURCES
    airline.cpp
    airport.cpp
    airportmodel.cpp
    atc.cpp
    atctracker.cpp
    client.cpp
    clientlist.cpp
    clientmodel.cpp
    fir.cpp
    firmodel.cpp
    flightplan.cpp
    flighttracker.cpp
    geo.cpp
    lonlat.cpp
    metar.cpp
    metarlistmodel.cpp
    natparser.cpp
    pilot.cpp
    servertracker.cpp
)

set (core_HEADERS
    airline.h
    airport.h
    airportmodel.h
    atc.h
    atctracker.h
    client.h
    clientlist.h
    clientmodel.h
    fir.h
    firmodel.h
    flightplan.h
    flighttracker.h
    geo.h
    lonlat.h
    metar.h
    metarlistmodel.h
    natparser.h
    pilot.h
    servertracker.h
    
    core.h
)

set (core_PRIVATE_HEADERS

)

add_library (v8rcore SHARED ${core_SOURCES} ${core_HEADERS} ${core_PRIVATE_HEADERS})
target_link_libraries (v8rcore Qt5::Core Qt5::Network)
set_target_properties (v8rcore PROPERTIES CXX_VISIBILITY_PRESET hidden)

include (GenerateExportHeader)
generate_export_header(v8rcore)

install (TARGETS v8rcore
    EXPORT v8rcore
    RUNTIME DESTINATION ${vatsinator_BINARY_TARGET_DIR}
    LIBRARY DESTINATION ${vatsinator_LIBRARY_TARGET_DIR}
    ARCHIVE DESTINATION ${vatsinator_ARCHIVE_TARGET_DIR} CONFIGURATIONS Debug
)

add_subdirectory (resources)
