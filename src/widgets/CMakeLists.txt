find_package (Qt5Core REQUIRED)
find_package (Qt5Network REQUIRED)
find_package (Qt5Gui REQUIRED)
find_package (Qt5Widgets REQUIRED)

include_directories (${CMAKE_CURRENT_BINARY_DIR})

set (widgets_SOURCES
    airportdetailswindow.cpp
    atcdetailswindow.cpp
    clientlistwindow.cpp
    firdetailswindow.cpp
    flightdetailswindow.cpp
    flightprogresswidget.cpp
    mapinfowidget.cpp
    mapwidget.cpp
    metarwindow.cpp
    
    widgetsprivate.cpp
)

set (widgets_HEADERS
    airportdetailswindow.h
    atcdetailswindow.h
    clientlistwindow.h
    firdetailswindow.h
    flightdetailswindow.h
    flightprogresswidget.h
    mapinfowidget.h
    mapwidget.h
    metarwindow.h
    
    widgets.h
    widgetsexport.h
    widgetsprivate.h
)

set (widgets_UIS
    airportdetailswindow.ui
    atcdetailswindow.ui
    clientlistwindow.ui
    firdetailswindow.ui
    flightdetailswindow.ui
    metarwindow.ui
)

add_library (vtrwidgets SHARED ${widgets_SOURCES} ${widgets_HEADERS} ${widgets_UIS})
target_link_libraries (vtrwidgets vtrgui vtrmisc Qt5::Widgets)
set_target_properties (vtrwidgets PROPERTIES CXX_VISIBILITY_PRESET hidden)
install (TARGETS vtrwidgets
    EXPORT vtrwidgets
    RUNTIME DESTINATION bin CONFIGURATIONS Debug Release
    LIBRARY DESTINATION lib/vatsinator CONFIGURATIONS Debug
    ARCHIVE DESTINATION lib CONFIGURATIONS Debug
)
