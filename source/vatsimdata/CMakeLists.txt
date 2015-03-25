set (vatsimdata_SRCS
  abstractbookingprovider.cpp
  abstractnotamprovider.cpp
  airport.cpp
  atcbooking.cpp
  client.cpp
  controller.cpp
  euroutenotamprovider.cpp
  fir.cpp
  lonlat.cpp
  metar.cpp
  notam.cpp
  pilot.cpp
  tma.cpp
  uir.cpp
  updatescheduler.cpp
  vatbookbookingprovider.cpp
  vatsimdatadocument.cpp
  vatsimdatahandler.cpp
  vatsimstatusdocument.cpp
)

set (vatsimdata_HEADERS
  abstractbookingprovider.h
  abstractnotamprovider.h
  airport.h
  atcbooking.h
  client.h
  controller.h
  euroutenotamprovider.h
  fir.h
  lonlat.h
  metar.h
  notam.h
  pilot.h
  tma.h
  uir.h
  updatescheduler.h
  vatbookbookingprovider.h
  vatsimdatadocument.h
  vatsimdatahandler.h
  vatsimstatusdocument.h
)

add_library (vatsimdata OBJECT
  ${vatsimdata_SRCS}
  ${vatsimdata_HEADERS}
)

add_dependencies (vatsimdata ui)
