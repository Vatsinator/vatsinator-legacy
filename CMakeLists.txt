#
# Copyright (C) 2012  Michał Garapich garrappachc@gmail.com
#

cmake_minimum_required(VERSION 2.6)
project(vatsinator)

find_package(Qt4 REQUIRED)
find_package(OpenGL REQUIRED)

if (CMAKE_BUILD_TYPE MATCHES Debug)
	set(CMAKE_BUILD_TYPE Debug)
else (CMAKE_BUILD_TYPE MATCHES Debug)
	set(CMAKE_BUILD_TYPE Release)
endif (CMAKE_BUILD_TYPE MATCHES Debug)



set(CMAKE_CXX_FLAGS_RELEASE "-std=c++0x -DNO_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -std=c++0x -fpermissive")

set(vatsinator_SOURCES
	main.cpp
	src/AirportDetailsWindow.cpp
	src/AirportObject.cpp
	src/AirportsDatabase.cpp
	src/ATCDetailsWindow.cpp
	src/ClientDetailsAction.cpp
	src/ColorButton.cpp
	src/Controller.cpp
	src/DetailsButton.cpp
	src/Fir.cpp
	src/FirsDatabase.cpp
	src/FlightDetailsWindow.cpp
	src/HttpHandler.cpp
	src/MapWidget.cpp
	src/Metar.cpp
	src/MetarAction.cpp
	src/MetarsHandler.cpp
	src/MetarsWindow.cpp
	src/Pilot.cpp
	src/SettingsWindow.cpp
	src/ShowButton.cpp
	src/TrackAction.cpp
	src/Uir.cpp
	src/UserInterface.cpp
	src/VatsimDataHandler.cpp
	src/VatsinatorApplication.cpp
)


if (CMAKE_BUILD_TYPE MATCHES Debug)
	set(vatsinator_SOURCES
		${vatsinator_SOURCES} src/MemoryTracker.cpp
	)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

set(vatsinator_QT_HEADERS
	include/AirportDetailsWindow.h
	include/ATCDetailsWindow.h
	include/ClientDetailsAction.h
	include/ColorButton.h
	include/DetailsButton.h
	include/FlightDetailsWindow.h
	include/HttpHandler.h
	include/MapWidget.h
	include/MetarAction.h
	include/MetarsHandler.h
	include/MetarsWindow.h
	include/SettingsWindow.h
	include/ShowButton.h
	include/TrackAction.h
	include/UserInterface.h
	include/VatsimDataHandler.h
	include/VatsinatorApplication.h
)

set(vatsinator_FORMS
	resources/AirportDetailsWindow.ui
	resources/ATCDetailsWindow.ui
	resources/FlightDetailsWindow.ui
	resources/MetarsWindow.ui
	resources/SettingsWindow.ui
	resources/UserInterface.ui
)

set(vatsinator_RESOURCES
	resources/imgs.qrc
)

set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTNETWORK TRUE)

qt4_wrap_cpp(vatsinator_HEADERS_MOC ${vatsinator_QT_HEADERS})
qt4_wrap_ui(vatsinator_FORMS_HEADERS ${vatsinator_FORMS})
qt4_add_resources(vatsinator_RESOURCES_RCC ${vatsinator_RESOURCES})

include(${QT_USE_FILE})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(include)
add_definitions(${QT_DEFINITIONS})

add_executable(vatsinator
	${vatsinator_SOURCES}
	${vatsinator_HEADERS_MOC}
	${vatsinator_FORMS_HEADERS}
	${vatsinator_RESOURCES_RCC}
)

if (MINGW)
	set(vatsinator_LIBS
		mingw32
		${OPENGL_gl_LIBRARY}
		${QT_QTCORE_LIBRARY}
		${QT_QTGUI_LIBRARY}
		${QT_QTOPENGL_LIBRARY}
		${QT_QTNETWORK_LIBRARY}
	)
else (MINGW)
	set_property(
		TARGET vatsinator
		PROPERTY COMPILE_DEFINITIONS PREFIX="${CMAKE_INSTALL_PREFIX}"
	)
	
	set(vatsinator_LIBS
		${QT_LIBRARIES}
		${OPENGL_LIBRARY}
	)
endif (MINGW)

target_link_libraries(vatsinator ${vatsinator_LIBS})

install(FILES
	resources/WorldAirports.db
	resources/WorldFirs.db
	resources/vatsinator.dat
	DESTINATION share/vatsinator)

install(PROGRAMS
	${CMAKE_CURRENT_BINARY_DIR}/vatsinator
	DESTINATION bin)

