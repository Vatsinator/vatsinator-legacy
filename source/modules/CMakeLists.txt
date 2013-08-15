vatsinator_start_subdirectory (modules)

set (modules_SRCS
    airporttracker.cpp
    dataupdater.cpp
    flighttracker.cpp
    homelocation.cpp
    modelmatcher.cpp
    modulemanager.cpp
    updatechecker.cpp
    vatbookhandler.cpp
    
    models/bookedatctablemodel.cpp
    
    vatbook/bookedcontroller.cpp
)

vatsinator_subdirectory_sources (modules "${modules_SRCS}")

set (modules_MOC_SRCS
    airporttracker.h
    dataupdater.h
    flighttracker.h
    homelocation.h
    modulemanager.h
    updatechecker.h
    vatbookhandler.h
    
    models/bookedatctablemodel.h
)

vatsinator_subdirectory_mocs (modules "${modules_MOC_SRCS}")

vatsinator_end_subdirectory (modules)

add_dependencies (modules ui)
