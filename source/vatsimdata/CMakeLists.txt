vatsinator_start_subdirectory (vatsimdata)

set (vatsimdata_SRCS
    abstractbookingprovider.cpp
    airport.cpp
    atcbooking.cpp
    client.cpp
    controller.cpp
    fir.cpp
    lonlat.cpp
    metar.cpp
    notam.cpp
    pilot.cpp
    uir.cpp
    updatescheduler.cpp
    vatbookbookingprovider.cpp
    vatsimdatahandler.cpp
    
    models/airporttablemodel.cpp
    models/atcbookingtablemodel.cpp
    models/controllertablemodel.cpp
    models/flighttablemodel.cpp
    models/metarlistmodel.cpp
    models/notamlistmodel.cpp
)
vatsinator_subdirectory_sources (vatsimdata "${vatsimdata_SRCS}")

vatsinator_end_subdirectory (vatsimdata)

add_dependencies (vatsimdata ui)
