vatsinator_start_subdirectory (db)

set (db_SRCS
    airline.cpp
    airlinedatabase.cpp
    airportdatabase.cpp
    firdatabase.cpp
    worldmap.cpp
)

vatsinator_subdirectory_sources (db "${db_SRCS}")

vatsinator_end_subdirectory (db)

