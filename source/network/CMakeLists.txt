vatsinator_start_subdirectory (network)

set (network_SRCS
    abstractnotamprovider.cpp
    dataupdater.cpp
    euroutenotamprovider.cpp
    filedownloader.cpp
    plaintextdownloader.cpp
    resourcemanager.cpp
    statspurveyor.cpp
)

vatsinator_subdirectory_sources (network "${network_SRCS}")

vatsinator_end_subdirectory (network)

add_dependencies (network ui)

