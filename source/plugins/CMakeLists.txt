vatsinator_start_subdirectory (plugins)

set (plugins_SRCS
  weatherdata.cpp
  weatherforecastreply.cpp
  weatherforecastrequest.cpp
)
vatsinator_subdirectory_sources (plugins "${plugins_SRCS}")

set (plugins_HEADERS
  weatherdata.h
  weatherforecastinterface.h
  weatherforecastreply.h
  weatherforecastrequest.h
)
vatsinator_subdirectory_headers (plugins "${plugins_HEADERS}")

set (plugins_QRCS
  resources/weather.qrc
)
vatsinator_subdirectory_resources (plugins "${plugins_QRCS}")

vatsinator_end_subdirectory (plugins)