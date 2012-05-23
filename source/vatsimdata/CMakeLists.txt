vatsinator_start_subdirectory (vatsimdata)

set (vatsimdata_SRCS
	metar.cpp
	metarshandler.cpp
	vatsimdatahandler.cpp
)

vatsinator_subdirectory_sources (vatsimdata "${vatsimdata_SRCS}")

set (vatsimdata_MOC_SRCS
	metarshandler.h
	vatsimdatahandler.h
)

vatsinator_subdirectory_mocs (vatsimdata "${vatsimdata_MOC_SRCS}")

vatsinator_end_subdirectory (vatsimdata)


