set (ui_SRCS
	AirportDetailsAction.cpp
	AirportDetailsWindow.cpp
	ATCDetailsWindow.cpp
	ClientDetailsAction.cpp
	ColorButton.cpp
	DetailsButton.cpp
	FlightDetailsWindow.cpp
	MetarAction.cpp
	MetarsWindow.cpp
	SettingsWindow.cpp
	ShowButton.cpp
	TrackAction.cpp
	UserInterface.cpp
	
	mapwidget/AirportObject.cpp
	mapwidget/Controller.cpp
	mapwidget/Fir.cpp
	mapwidget/MapWidget.cpp
	mapwidget/Pilot.cpp
	mapwidget/Uir.cpp
)

set (ui_MOC_SRCS
	AirportDetailsAction.h
	AirportDetailsWindow.h
	ATCDetailsWindow.h
	ClientDetailsAction.h
	ColorButton.h
	DetailsButton.h
	FlightDetailsWindow.h
	MetarAction.h
	MetarsWindow.h
	SettingsWindow.h
	ShowButton.h
	TrackAction.h
	UserInterface.h
	
	mapwidget/MapWidget.h
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

