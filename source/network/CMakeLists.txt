vatsinator_start_subdirectory (network)

set (network_SRCS
	HttpHandler.cpp
)

vatsinator_subdirectory_sources (network "${network_SRCS}")

set (network_MOC_SRCS
	HttpHandler.h
)

vatsinator_subdirectory_mocs (network "${network_MOC_SRCS}")

vatsinator_end_subdirectory (network)

