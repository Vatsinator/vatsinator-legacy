find_package (Qt5 5.7 REQUIRED COMPONENTS Core Network Gui Widgets)

set (widgets_SOURCES
    airportdetailswindow.cpp
    atcdetailswindow.cpp
    clientlistwindow.cpp
    firdetailswindow.cpp
    flightdetailswindow.cpp
    flightprogresswidget.cpp
    plugingroupboxwidget.cpp
    pluginwidget.cpp
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
    plugingroupboxwidget.h
    pluginwidget.h
    mapinfowidget.h
    mapwidget.h
    metarwindow.h
    
    widgets.h
    widgetsprivate.h
)

set (widgets_UIS
    airportdetailswindow.ui
    atcdetailswindow.ui
    clientlistwindow.ui
    firdetailswindow.ui
    flightdetailswindow.ui
    pluginwidget.ui
    metarwindow.ui
)

add_library (v8rwidgets SHARED ${widgets_SOURCES} ${widgets_HEADERS} ${widgets_UIS})
target_link_libraries (v8rwidgets v8rgui v8rmisc Qt5::Widgets)
set_target_properties (v8rwidgets PROPERTIES CXX_VISIBILITY_PRESET hidden)

include (GenerateExportHeader)
generate_export_header(v8rwidgets)

install (TARGETS v8rwidgets
    EXPORT v8rwidgets
    RUNTIME DESTINATION ${vatsinator_BINARY_TARGET_DIR}
    LIBRARY DESTINATION ${vatsinator_LIBRARY_TARGET_DIR}
    ARCHIVE DESTINATION ${vatsinator_ARCHIVE_TARGET_DIR} CONFIGURATIONS Debug
)
