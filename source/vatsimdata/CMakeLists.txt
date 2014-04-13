vatsinator_start_subdirectory (vatsimdata)

set (vatsimdata_SRCS
    airline.cpp
    airlinemanager.cpp
    airport.cpp
    client.cpp
    fir.cpp
    metar.cpp
    notam.cpp
    uir.cpp
    updatescheduler.cpp
    vatsimdatahandler.cpp
    
    airport/activeairport.cpp
    airport/emptyairport.cpp
    
    client/controller.cpp
    client/pilot.cpp
    
    models/airporttablemodel.cpp
    models/controllertablemodel.cpp
    models/flighttablemodel.cpp
    models/metarlistmodel.cpp
    models/notamlistmodel.cpp
)

vatsinator_subdirectory_sources (vatsimdata "${vatsimdata_SRCS}")

set (vatsimdata_MOC_SRCS
    airline.h
    airlinemanager.h
    updatescheduler.h
    vatsimdatahandler.h
    
    models/airporttablemodel.h
    models/controllertablemodel.h
    models/flighttablemodel.h
    models/metarlistmodel.h
    models/notamlistmodel.h
)

vatsinator_subdirectory_mocs (vatsimdata "${vatsimdata_MOC_SRCS}")

vatsinator_end_subdirectory (vatsimdata)

add_dependencies (vatsimdata ui)

