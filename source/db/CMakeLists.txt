set (db_SRCS
  airline.cpp
  airlinedatabase.cpp
  airportdatabase.cpp
  firdatabase.cpp
  worldmap.cpp
)

set (db_HEADERS
  airline.h
  airlinedatabase.h
  airportdatabase.h
  firdatabase.h
  point.h
  worldmap.h
)

add_library (db OBJECT
  ${db_SRCS}
  ${db_HEADERS}
)
