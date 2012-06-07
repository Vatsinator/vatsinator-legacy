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
	userinterface.cpp
	
	actions/actionmenuseparator.cpp
	actions/airportdetailsaction.cpp
	actions/clientdetailsaction.cpp
	actions/firdetailsaction.cpp
	actions/metaraction.cpp
	actions/trackaction.cpp
	
	buttons/colorbutton.cpp
	buttons/clientdetailsbutton.cpp
	buttons/showairportbutton.cpp
	
	mapwidget/mapwidget.cpp
	
	windows/airportdetailswindow.cpp
	windows/atcdetailswindow.cpp
	windows/atclistwindow.cpp
	windows/firdetailswindow.cpp
	windows/flightdetailswindow.cpp
	windows/flightslistwindow.cpp
	windows/metarswindow.cpp
	windows/settingswindow.cpp
)

vatsinator_subdirectory_sources (ui "${ui_SRCS}")

set (ui_MOC_SRCS
	userinterface.h
	
	actions/actionmenuseparator.h
	actions/airportdetailsaction.h
	actions/clientdetailsaction.h
	actions/firdetailsaction.h
	actions/metaraction.h
	actions/trackaction.h
	
	buttons/colorbutton.h
	buttons/clientdetailsbutton.h
	buttons/showairportbutton.h
	
	mapwidget/mapwidget.h
	
	windows/airportdetailswindow.h
	windows/atcdetailswindow.h
	windows/atclistwindow.h
	windows/firdetailswindow.h
	windows/flightdetailswindow.h
	windows/flightslistwindow.h
	windows/metarswindow.h
	windows/settingswindow.h
)

vatsinator_subdirectory_mocs (ui "${ui_MOC_SRCS}")

set (ui_FORMS
	resources/airportdetailswindow.ui
	resources/atcdetailswindow.ui
	resources/atclistwindow.ui
	resources/firdetailswindow.ui
	resources/flightdetailswindow.ui
	resources/flightslistwindow.ui
	resources/metarswindow.ui
	resources/settingswindow.ui
	resources/userinterface.ui
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
