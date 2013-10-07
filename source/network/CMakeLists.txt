vatsinator_start_subdirectory (network)

file (READ ${CMAKE_CURRENT_SOURCE_DIR}/resources/ultimate-weather-forecasts.key
  ultimate_weather_forecasts_API_KEY)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/ultimate-weather-forecasts.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/ultimate-weather-forecasts.h
)

set (network_SRCS
    filedownloader.cpp
    plaintextdownloader.cpp
    resourcemanager.cpp
    statspurveyor.cpp
    weatherforecast.cpp
)

vatsinator_subdirectory_sources (network "${network_SRCS}")

set (network_MOC_SRCS
    filedownloader.h
    plaintextdownloader.h
    resourcemanager.h
    statspurveyor.h
    weatherforecast.h
)

vatsinator_subdirectory_mocs (network "${network_MOC_SRCS}")

vatsinator_end_subdirectory (network)

add_dependencies (network ui)

