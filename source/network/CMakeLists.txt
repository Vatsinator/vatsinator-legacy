set (network_SRCS
  dataupdater.cpp
  filedownloader.cpp
  plaintextdownloader.cpp
  resourcemanager.cpp
  statspurveyor.cpp
  weatherforecastdownloader.cpp
)

set (network_HEADERS
  dataupdater.h
  filedownloader.h
  plaintextdownloader.h
  resourcemanager.h
  statspurveyor.h
  weatherforecastdownloader.h
)

add_library (network OBJECT
  ${network_SRCS}
  ${network_HEADERS}
)

add_dependencies (network ui)
