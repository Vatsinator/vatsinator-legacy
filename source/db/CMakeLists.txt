set (db_SRCS
  airline.cpp
  airlinedatabase.cpp
  airportdatabase.cpp
  firdatabase.cpp
)

set (db_HEADERS
  airline.h
  airlinedatabase.h
  airportdatabase.h
  firdatabase.h
  point.h
)

add_library (db OBJECT
  ${db_SRCS}
  ${db_HEADERS}
)
