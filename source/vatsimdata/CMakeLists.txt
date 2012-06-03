vatsinator_start_subdirectory (vatsimdata)

set (vatsimdata_SRCS
	airportobject.cpp
	controller.cpp
	fir.cpp
	metar.cpp
	pilot.cpp
	uir.cpp
	vatsimdatahandler.cpp
	
	models/metarlistmodel.cpp
)

vatsinator_subdirectory_sources (vatsimdata "${vatsimdata_SRCS}")

set (vatsimdata_MOC_SRCS
	vatsimdatahandler.h
	
	models/metarlistmodel.h
)

vatsinator_subdirectory_mocs (vatsimdata "${vatsimdata_MOC_SRCS}")

vatsinator_end_subdirectory (vatsimdata)


