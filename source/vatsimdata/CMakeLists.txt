vatsinator_start_subdirectory (vatsimdata)

set (vatsimdata_SRCS
    airport.cpp
    client.cpp
    fir.cpp
    lonlat.cpp
    metar.cpp
    notam.cpp
    uir.cpp
    updatescheduler.cpp
    vatsimdatahandler.cpp
    
    client/controller.cpp
    client/pilot.cpp
    
    models/airporttablemodel.cpp
    models/controllertablemodel.cpp
    models/flighttablemodel.cpp
    models/metarlistmodel.cpp
    models/notamlistmodel.cpp
)
vatsinator_subdirectory_sources (vatsimdata "${vatsimdata_SRCS}")

vatsinator_end_subdirectory (vatsimdata)

add_dependencies (vatsimdata ui)
