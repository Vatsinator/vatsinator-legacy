vatsinator_start_subdirectory (plugins)

set (plugins_SRCS
    weatherdata.cpp
    weatherforecastreply.cpp
    weatherforecastrequest.cpp
)
vatsinator_subdirectory_sources (plugins "${plugins_SRCS}")

set (plugins_QRCS
    resources/weather.qrc
)
vatsinator_subdirectory_resources (plugins "${plugins_QRCS}")

vatsinator_end_subdirectory (plugins)