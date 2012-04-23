set (network_SRCS
	HttpHandler.cpp
)

set (network_MOC_SRCS
	HttpHandler.h
)

vatsinator_subdirectory (network "${network_SRCS}" "${network_MOC_SRCS}" "" "")

