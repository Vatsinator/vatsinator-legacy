vatsinator_start_subdirectory (network)

set (network_SRCS
    filedownloader.cpp
    plaintextdownloader.cpp
    resourcemanager.cpp
    statspurveyor.cpp
    weatherforecast.cpp
    
    models/weatherforecastmodel.cpp
)

vatsinator_subdirectory_sources (network "${network_SRCS}")

set (network_MOC_SRCS
    filedownloader.h
    plaintextdownloader.h
    resourcemanager.h
    statspurveyor.h
    weatherforecast.h
    
    models/weatherforecastmodel.h
)

vatsinator_subdirectory_mocs (network "${network_MOC_SRCS}")

set (network_QRCS
    resources/weather.qrc
)

vatsinator_subdirectory_resources (network "${network_QRCS}")

vatsinator_end_subdirectory (network)

add_dependencies (network ui)

