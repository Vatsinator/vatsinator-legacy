set (plugins_SRCS
  atcbooking.cpp
  atcbookingtablemodel.cpp
  notam.cpp
  notamlistmodel.cpp
  weatherdata.cpp
  weatherforecastreply.cpp
  weatherforecastrequest.cpp
)

set (plugins_HEADERS
  atcbooking.h
  atcbookingtablemodel.h
  notam.h
  notamlistmodel.h
  notamprovider.h
  weatherdata.h
  weatherforecastreply.h
  weatherforecastrequest.h
  
  bookingprovider.h
  mapdrawer.h
  weatherforecastinterface.h
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

macro (vatsinator_static_plugin libname pluginname)
  set (vatsinator_STATIC_PLUGINS
    ${vatsinator_STATIC_PLUGINS}
    ${libname}
    PARENT_SCOPE)
  set (vatsinator_STATIC_PLUGIN_NAMES
    ${vatsinator_STATIC_PLUGIN_NAMES}
    ${pluginname}
    PARENT_SCOPE)
endmacro ()

add_subdirectory (euroute-notam-provider)
add_subdirectory (ultimate-weather-forecasts)
add_subdirectory (vatbook-booking-provider)
add_subdirectory (vector-map)

set (vatsinator_STATIC_PLUGINS
  ${vatsinator_STATIC_PLUGINS}
  PARENT_SCOPE
)

file (REMOVE "${CMAKE_CURRENT_BINARY_DIR}/import_static_plugins.h")
foreach (plugin_name ${vatsinator_STATIC_PLUGIN_NAMES})
  file (APPEND "${CMAKE_CURRENT_BINARY_DIR}/import_static_plugins.h" "Q_IMPORT_PLUGIN(${plugin_name})\n")
endforeach ()