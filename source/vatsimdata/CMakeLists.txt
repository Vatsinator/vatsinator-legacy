set (vatsimdata_SRCS
	Metar.cpp
	MetarsHandler.cpp
	VatsimDataHandler.cpp
)

set (vatsimdata_MOC_SRCS
	MetarsHandler.h
	VatsimDataHandler.h
)

vatsinator_subdirectory (vatsimdata "${vatsimdata_SRCS}" "${vatsimdata_MOC_SRCS}" "" "")


