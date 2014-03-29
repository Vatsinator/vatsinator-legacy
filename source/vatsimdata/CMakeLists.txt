vatsinator_start_subdirectory (vatsimdata)

set (vatsimdata_SRCS
    airport.cpp
    client.cpp
    fir.cpp
    lonlat.cpp
    metar.cpp
    uir.cpp
    vatsimdatahandler.cpp
    
    airport/activeairport.cpp
    airport/emptyairport.cpp
    
    client/controller.cpp
    client/pilot.cpp
    
    models/airporttablemodel.cpp
    models/controllertablemodel.cpp
    models/flighttablemodel.cpp
    models/metarlistmodel.cpp
)

vatsinator_subdirectory_sources (vatsimdata "${vatsimdata_SRCS}")

set (vatsimdata_MOC_SRCS
    vatsimdatahandler.h
    
    models/airporttablemodel.h
    models/controllertablemodel.h
    models/flighttablemodel.h
    models/metarlistmodel.h
)

vatsinator_subdirectory_mocs (vatsimdata "${vatsimdata_MOC_SRCS}")

vatsinator_end_subdirectory (vatsimdata)

add_dependencies (vatsimdata ui)

