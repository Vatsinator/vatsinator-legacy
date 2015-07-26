if (NOT vatsinator_ABOUT)
    file (READ ${CMAKE_CURRENT_SOURCE_DIR}/resources/about vatsinator_ABOUT)
    string (REPLACE "\n" "" vatsinator_ABOUT "${vatsinator_ABOUT}")
    string (REPLACE "\"" "\\\"" vatsinator_ABOUT "${vatsinator_ABOUT}")
    string (REPLACE "VATSINATOR_VERSION" "${vatsinator_VERSION}" vatsinator_ABOUT "${vatsinator_ABOUT}")
endif ()

if (NOT vatsinator_CHANGELOG)
    file (READ ${CMAKE_SOURCE_DIR}/Changelog vatsinator_CHANGELOG)
    string (REPLACE "<" "&lt;" vatsinator_CHANGELOG "${vatsinator_CHANGELOG}")
    string (REPLACE ">" "&gt;" vatsinator_CHANGELOG "${vatsinator_CHANGELOG}")
    string (REPLACE "\n" "<br>" vatsinator_CHANGELOG "${vatsinator_CHANGELOG}")
    string (REPLACE "\"" "\\\"" vatsinator_CHANGELOG "${vatsinator_CHANGELOG}")
    string (REPLACE "*" "<span style=\\\"color:#7893A2;\\\">*</span>" vatsinator_CHANGELOG "${vatsinator_CHANGELOG}") 
endif (NOT vatsinator_CHANGELOG)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/about.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/about.h
)

set (ui_SRCS
    notifiable.cpp
    userinterface.cpp
    
    map/airportitem.cpp
    map/firarea.cpp
    map/firitem.cpp
    map/flightitem.cpp
    map/maparea.cpp
    map/mapconfig.cpp
    map/mapitem.cpp
    map/maprenderer.cpp
    map/mapscene.cpp
    map/modelpixmapprovider.cpp
    map/uiritem.cpp
    map/worldtransform.cpp
    
    models/airporttablemodel.cpp
    models/atctablemodel.cpp
    models/flighttablemodel.cpp
    models/metarlistmodel.cpp
)

set (ui_HEADERS
    notifiable.h
    userinterface.h
    
    map/airportitem.h
    map/firarea.h
    map/firitem.h
    map/flightitem.h
    map/maparea.h
    map/mapconfig.h
    map/mapitem.h
    map/maprenderer.h
    map/mapscene.h
    map/modelpixmapprovider.h
    map/uiritem.h
    map/worldtransform.h
    
    models/airporttablemodel.h
    models/atctablemodel.h
    models/flighttablemodel.h
    models/metarlistmodel.h
)

set (widgets_SRCS
    vatsinatorstyle.cpp
    widgetsuserinterface.cpp
    
    actions/actionmenuseparator.cpp
    actions/airportdetailsaction.cpp
    actions/clientdetailsaction.cpp
    actions/firdetailsaction.cpp
    actions/metaraction.cpp
    actions/toggleinboundoutboundlinesaction.cpp
    actions/trackaction.cpp
    
    buttons/airportdetailsbutton.cpp
    buttons/colorbutton.cpp
    buttons/clientdetailsbutton.cpp
    buttons/urlbutton.cpp
    
    dialogs/apprestartdialog.cpp
    dialogs/datafetcherrordialog.cpp
    dialogs/letsendstatsdialog.cpp
    dialogs/newversiondialog.cpp
    dialogs/statusfetcherrordialog.cpp
    dialogs/vatsimmessagedialog.cpp
    
    pages/mappage.cpp
    pages/miscellaneouspage.cpp
    pages/networkpage.cpp
    pages/viewpage.cpp
    
    views/delayedmodeltableview.cpp
    views/resizablelistview.cpp
    
    widgets/delayedwidget.cpp
    widgets/flightprogresswidget.cpp
    widgets/infolabel.cpp
    widgets/mapwidget.cpp
    widgets/replaceablewidget.cpp
    widgets/scrolllabel.cpp
    widgets/weatherforecastwidget.cpp
    widgets/widgetsettingsmodule.cpp
    
    windows/aboutwindow.cpp
    windows/airportdetailswindow.cpp
    windows/atcdetailswindow.cpp
    windows/atclistwindow.cpp
    windows/basewindow.cpp
    windows/databasewindow.cpp
    windows/firdetailswindow.cpp
    windows/flightdetailswindow.cpp
    windows/flightlistwindow.cpp
    windows/metarswindow.cpp
    windows/settingswindow.cpp
    windows/vatsinatorwindow.cpp
)

set (quick_SRCS
    quickuserinterface.cpp
    
    quick/androidbridge.cpp
    quick/map.cpp
    quick/paletteprovider.cpp
)

set (widgets_HEADERS
    vatsinatorstyle.h
    widgetsuserinterface.h
    
    actions/actionmenuseparator.h
    actions/airportdetailsaction.h
    actions/clientdetailsaction.h
    actions/firdetailsaction.h
    actions/metaraction.h
    actions/toggleinboundoutboundlinesaction.h
    actions/trackaction.h
    
    buttons/airportdetailsbutton.h
    buttons/colorbutton.h
    buttons/clientdetailsbutton.h
    buttons/urlbutton.h
    
    dialogs/apprestartdialog.h
    dialogs/datafetcherrordialog.h
    dialogs/letsendstatsdialog.h
    dialogs/newversiondialog.h
    dialogs/statusfetcherrordialog.h
    dialogs/vatsimmessagedialog.h
    
    pages/mappage.h
    pages/miscellaneouspage.h
    pages/networkpage.h
    pages/viewpage.h
    
    views/delayedmodeltableview.h
    views/resizablelistview.h
    
    widgets/delayedwidget.h
    widgets/flightprogresswidget.h
    widgets/infolabel.h
    widgets/mapwidget.h
    widgets/replaceablewidget.h
    widgets/scrolllabel.h
    widgets/weatherforecastwidget.h
    widgets/widgetsettingsmodule.h
    
    windows/aboutwindow.h
    windows/airportdetailswindow.h
    windows/atcdetailswindow.h
    windows/atclistwindow.h
    windows/basewindow.h
    windows/databasewindow.h
    windows/firdetailswindow.h
    windows/flightdetailswindow.h
    windows/flightlistwindow.h
    windows/metarswindow.h
    windows/settingswindow.h
    windows/vatsinatorwindow.h
)

set (quick_HEADERS
    quickuserinterface.h
    
    quick/androidbridge.h
    quick/map.h
    quick/paletteprovider.h
)

set (ui_QRCS
    resources/files.qrc
    resources/imgs.qrc
    resources/pixmaps.qrc
    resources/shaders.qrc
)
qt5_add_resources (ui_RESOURCES ${ui_QRCS})

set (quick_QRCS
    qml/qmls.qrc
)

if (${vatsinator_UI_IMPLEMENTATION} MATCHES "widgets")
    set (impl_SRCS ${widgets_SRCS})
    set (impl_HEADERS ${widgets_HEADERS})
elseif (${vatsinator_UI_IMPLEMENTATION} MATCHES "quick")
    set (impl_SRCS ${quick_SRCS})
    set (impl_HEADERS ${quick_HEADERS})
    qt5_add_resources (ui_RESOURCES ${quick_QRCS})
endif ()

add_library (ui OBJECT
    ${ui_SRCS}
    ${ui_HEADERS}
    ${ui_RESOURCES}
    ${impl_SRCS}
    ${impl_HEADERS}
)
