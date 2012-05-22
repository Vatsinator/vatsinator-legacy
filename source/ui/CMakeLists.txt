vatsinator_start_subdirectory (ui)

if (NOT vatsinator_ABOUT)
	file (READ ${CMAKE_CURRENT_SOURCE_DIR}/resources/about vatsinator_ABOUT)
	string (REPLACE "\n" "" vatsinator_ABOUT ${vatsinator_ABOUT})
	string (REPLACE "\"" "\\\"" vatsinator_ABOUT ${vatsinator_ABOUT})
	string (REPLACE "VATSINATOR_VERSION" "${vatsinator_VERSION}" vatsinator_ABOUT ${vatsinator_ABOUT})
endif (NOT vatsinator_ABOUT)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/about.h.cmake
	${CMAKE_CURRENT_BINARY_DIR}/about.h
)

set (ui_SRCS
	UserInterface.cpp
	
	actions/ActionMenuSeparator.cpp
	actions/AirportDetailsAction.cpp
	actions/ClientDetailsAction.cpp
	actions/FirDetailsAction.cpp
	actions/MetarAction.cpp
	actions/TrackAction.cpp
	
	buttons/ColorButton.cpp
	buttons/DetailsButton.cpp
	buttons/ShowAirportButton.cpp
	buttons/ShowPilotButton.cpp
	
	mapwidget/AirportObject.cpp
	mapwidget/Controller.cpp
	mapwidget/Fir.cpp
	mapwidget/MapWidget.cpp
	mapwidget/Pilot.cpp
	mapwidget/Uir.cpp
	
	windows/AirportDetailsWindow.cpp
	windows/ATCDetailsWindow.cpp
	windows/FirDetailsWindow.cpp
	windows/FlightDetailsWindow.cpp
	windows/FlightsListWindow.cpp
	windows/MetarsWindow.cpp
	windows/SettingsWindow.cpp
)

vatsinator_subdirectory_sources (ui "${ui_SRCS}")

set (ui_MOC_SRCS
	UserInterface.h
	
	actions/ActionMenuSeparator.h
	actions/AirportDetailsAction.h
	actions/ClientDetailsAction.h
	actions/FirDetailsAction.h
	actions/MetarAction.h
	actions/TrackAction.h
	
	buttons/ShowAirportButton.h
	buttons/ColorButton.h
	buttons/DetailsButton.h
	buttons/ShowPilotButton.h
	
	mapwidget/MapWidget.h
	
	windows/AirportDetailsWindow.h
	windows/ATCDetailsWindow.h
	windows/FirDetailsWindow.h
	windows/FlightDetailsWindow.h
	windows/FlightsListWindow.h
	windows/MetarsWindow.h
	windows/SettingsWindow.h
)

vatsinator_subdirectory_mocs (ui "${ui_MOC_SRCS}")

set (ui_FORMS
	resources/AirportDetailsWindow.ui
	resources/ATCDetailsWindow.ui
	resources/FirDetailsWindow.ui
	resources/FlightDetailsWindow.ui
	resources/FlightsListWindow.ui
	resources/MetarsWindow.ui
	resources/SettingsWindow.ui
	resources/UserInterface.ui
)

vatsinator_subdirectory_forms (ui "${ui_FORMS}")

set (ui_QRCS
	resources/imgs.qrc
)

vatsinator_subdirectory_resources (ui "${ui_QRCS}")

# icon for Windows
if (MINGW)
	add_custom_command (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vatsinatorico.o
		COMMAND ${WINDRES_COMMAND}
		-i${CMAKE_CURRENT_SOURCE_DIR}/resources/vatsinator.rc
		-o ${CMAKE_CURRENT_BINARY_DIR}/vatsinatorico.o
	)
	
	vatsinator_subdirectory_sources (ui "${CMAKE_CURRENT_BINARY_DIR}/vatsinatorico.o")
endif (MINGW)

vatsinator_end_subdirectory (ui)

add_dependencies (ui vdebug)
