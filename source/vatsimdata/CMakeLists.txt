vatsinator_start_subdirectory (vatsimdata)

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
  uir.cpp
  updatescheduler.cpp
  vatbookbookingprovider.cpp
  vatsimdatadocument.cpp
  vatsimdatahandler.cpp
  vatsimstatusdocument.cpp
)
vatsinator_subdirectory_sources (vatsimdata "${vatsimdata_SRCS}")

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
  uir.h
  updatescheduler.h
  vatbookbookingprovider.h
  vatsimdatadocument.h
  vatsimdatahandler.h
  vatsimstatusdocument.h
)
vatsinator_subdirectory_headers (vatsimdata "${vatsimdata_HEADERS}")

vatsinator_end_subdirectory (vatsimdata)

add_dependencies (vatsimdata ui)
