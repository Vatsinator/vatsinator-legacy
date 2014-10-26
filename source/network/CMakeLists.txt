vatsinator_start_subdirectory (network)

set (network_SRCS
  dataupdater.cpp
  filedownloader.cpp
  plaintextdownloader.cpp
  resourcemanager.cpp
  statspurveyor.cpp
)
vatsinator_subdirectory_sources (network "${network_SRCS}")

set (network_HEADERS
  dataupdater.h
  filedownloader.h
  plaintextdownloader.h
  resourcemanager.h
  statspurveyor.h
)
vatsinator_subdirectory_headers (network "${network_HEADERS}")

vatsinator_end_subdirectory (network)

add_dependencies (network ui)
