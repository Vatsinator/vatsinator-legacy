vatsinator_start_subdirectory (vatsimdata)

set (vatsimdata_SRCS
	Metar.cpp
	MetarsHandler.cpp
	VatsimDataHandler.cpp
)

vatsinator_subdirectory_sources (vatsimdata "${vatsimdata_SRCS}")

set (vatsimdata_MOC_SRCS
	MetarsHandler.h
	VatsimDataHandler.h
)

vatsinator_subdirectory_mocs (vatsimdata "${vatsimdata_MOC_SRCS}")

vatsinator_end_subdirectory (vatsimdata)


