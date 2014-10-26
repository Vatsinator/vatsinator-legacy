vatsinator_start_subdirectory (db)

set (db_SRCS
  airline.cpp
  airlinedatabase.cpp
  airportdatabase.cpp
  firdatabase.cpp
  worldmap.cpp
)
vatsinator_subdirectory_sources (db "${db_SRCS}")

set (db_HEADERS
  airline.h
  airlinedatabase.h
  airportdatabase.h
  firdatabase.h
  point.h
  worldmap.h
)
vatsinator_subdirectory_headers (db "${db_HEADERS}")

vatsinator_end_subdirectory (db)

