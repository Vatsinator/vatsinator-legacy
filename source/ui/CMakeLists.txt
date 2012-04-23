set (ui_SRCS
	UserInterface.cpp
	
	actions/AirportDetailsAction.cpp
	actions/ClientDetailsAction.cpp
	actions/MetarAction.cpp
	actions/TrackAction.cpp
	
	buttons/ColorButton.cpp
	buttons/DetailsButton.cpp
	buttons/ShowButton.cpp
	
	mapwidget/AirportObject.cpp
	mapwidget/Controller.cpp
	mapwidget/Fir.cpp
	mapwidget/MapWidget.cpp
	mapwidget/Pilot.cpp
	mapwidget/Uir.cpp
	
	windows/AirportDetailsWindow.cpp
	windows/ATCDetailsWindow.cpp
	windows/FlightDetailsWindow.cpp
	windows/MetarsWindow.cpp
	windows/SettingsWindow.cpp
)

set (ui_MOC_SRCS
	UserInterface.h
	
	actions/AirportDetailsAction.h
	actions/ClientDetailsAction.h
	actions/MetarAction.h
	actions/TrackAction.h
	
	buttons/ColorButton.h
	buttons/DetailsButton.h
	buttons/ShowButton.h
	
	mapwidget/MapWidget.h
	
	windows/AirportDetailsWindow.h
	windows/ATCDetailsWindow.h
	windows/FlightDetailsWindow.h
	windows/MetarsWindow.h
	windows/SettingsWindow.h
)

set (ui_FORMS
	resources/AirportDetailsWindow.ui
	resources/ATCDetailsWindow.ui
	resources/FlightDetailsWindow.ui
	resources/MetarsWindow.ui
	resources/SettingsWindow.ui
	resources/UserInterface.ui
)

set (ui_QRCS
	resources/imgs.qrc
)

# icon for Windows
if (MINGW)
	add_custom_command (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vatsinatorico.o
		COMMAND ${WINDRES_COMMAND}
		-i${CMAKE_CURRENT_SOURCE_DIR}/resources/vatsinator.rc
		-o ${CMAKE_CURRENT_BINARY_DIR}/vatsinatorico.o
	)
	
	set (ui_SRCS
		${ui_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/vatsinatorico.o
	)
endif (MINGW)

vatsinator_subdirectory (ui "${ui_SRCS}" "${ui_MOC_SRCS}" "${ui_FORMS}" "${ui_QRCS}")
