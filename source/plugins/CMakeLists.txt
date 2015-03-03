set (plugins_SRCS
  weatherdata.cpp
  weatherforecastreply.cpp
  weatherforecastrequest.cpp
)

set (plugins_HEADERS
  weatherdata.h
  weatherforecastinterface.h
  weatherforecastreply.h
  weatherforecastrequest.h
)

set (plugins_QRCS
  resources/weather.qrc
)
qt5_add_resources (plugins_RESOURCES ${plugins_QRCS})

add_library (plugins OBJECT
  ${plugins_SRCS}
  ${plugins_HEADERS}
  ${plugins_RESOURCES}
)

add_subdirectory (ultimate-weather-forecasts)

set (vatsinator_STATIC_PLUGINS
  ultimateweatherforecasts
  
  PARENT_SCOPE
)
