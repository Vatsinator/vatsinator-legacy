vatsinator_start_subdirectory (db)

set (db_SRCS
    airline.cpp
    airlinedatabase.cpp
    airportdatabase.cpp
    firdatabase.cpp
    worldmap.cpp
)

vatsinator_subdirectory_sources (db "${db_SRCS}")

set (db_MOC_SRCS
    airline.h
    airlinedatabase.h
    airportdatabase.h
    firdatabase.h
    worldmap.h
)

vatsinator_subdirectory_mocs (db "${db_MOC_SRCS}")

vatsinator_end_subdirectory (db)

