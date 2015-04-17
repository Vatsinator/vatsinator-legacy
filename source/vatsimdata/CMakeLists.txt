set (vatsimdata_SRCS
  airport.cpp
  client.cpp
  controller.cpp
  fir.cpp
  lonlat.cpp
  metar.cpp
  pilot.cpp
  uir.cpp
  updatescheduler.cpp
  vatsimdatadocument.cpp
  vatsimdatahandler.cpp
  vatsimstatusdocument.cpp
)

set (vatsimdata_HEADERS
  airport.h
  client.h
  controller.h
  fir.h
  lonlat.h
  metar.h
  pilot.h
  uir.h
  updatescheduler.h
  vatsimdatadocument.h
  vatsimdatahandler.h
  vatsimstatusdocument.h
)

add_library (vatsimdata OBJECT
  ${vatsimdata_SRCS}
  ${vatsimdata_HEADERS}
)

add_dependencies (vatsimdata ui)
