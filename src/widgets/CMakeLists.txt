find_package (Qt5Widgets REQUIRED)
include_directories (${Qt5Widgets_INCLUDE_DIRS})

include_directories (${CMAKE_CURRENT_BINARY_DIR})

set (widgets_SOURCES
    airportdetailswindow.cpp
    atcdetailswindow.cpp
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

add_library (vtrwidgets SHARED ${widgets_SOURCES} ${widgets_HEADERS})
target_link_libraries (vtrwidgets Qt5::Widgets vtrgui vtrmisc)
set_target_properties (vtrwidgets PROPERTIES CXX_VISIBILITY_PRESET hidden)
install (TARGETS vtrwidgets
    EXPORT vtrwidgets
    RUNTIME DESTINATION bin CONFIGURATIONS Debug Release
    LIBRARY DESTINATION lib/vatsinator CONFIGURATIONS Debug
    ARCHIVE DESTINATION lib CONFIGURATIONS Debug
)
