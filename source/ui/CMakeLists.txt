vatsinator_start_subdirectory (ui)

if (NOT vatsinator_ABOUT)
    file (READ ${CMAKE_CURRENT_SOURCE_DIR}/resources/about vatsinator_ABOUT)
    string (REPLACE "\n" "" vatsinator_ABOUT "${vatsinator_ABOUT}")
    string (REPLACE "\"" "\\\"" vatsinator_ABOUT "${vatsinator_ABOUT}")
    string (REPLACE "VATSINATOR_VERSION" "${vatsinator_VERSION}" vatsinator_ABOUT "${vatsinator_ABOUT}")
endif (NOT vatsinator_ABOUT)

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
    map/approachcircleitem.cpp
    map/firitem.cpp
    map/flightitem.cpp
    map/iconkeeper.cpp
    map/mapconfig.cpp
    map/mapitem.cpp
    map/maprenderer.cpp
    map/mapscene.cpp
    map/modelmatcher.cpp
    map/uiritem.cpp
    map/worldpolygon.cpp
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
    pages/pluginlistwidgetitem.cpp
    pages/viewpage.cpp
    
    views/airporttableview.cpp
    views/controllertableview.cpp
    views/delayedmodeltableview.cpp
    views/flighttableview.cpp
    
    widgets/delayedwidget.cpp
    widgets/flightprogresswidget.cpp
    widgets/infolabel.cpp
    widgets/mapwidget.cpp
    widgets/replaceablewidget.cpp
    widgets/scrolllabel.cpp
    widgets/weatherforecastwidget.cpp
    
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

vatsinator_subdirectory_sources (ui "${ui_SRCS}")
if (${vatsinator_UI_IMPLEMENTATION} MATCHES "widgets")
  vatsinator_subdirectory_sources (ui "${widgets_SRCS}")
endif (${vatsinator_UI_IMPLEMENTATION} MATCHES "widgets")


set (widgets_FORMS
    dialogs/apprestartdialog.ui
    dialogs/letsendstatsdialog.ui
    dialogs/newversiondialog.ui
    dialogs/vatsimmessagedialog.ui
    
    pages/mappage.ui
    pages/miscellaneouspage.ui
    pages/networkpage.ui
    pages/viewpage.ui

    windows/aboutwindow.ui
    windows/airportdetailswindow.ui
    windows/atcdetailswindow.ui
    windows/atclistwindow.ui
    windows/databasewindow.ui
    windows/firdetailswindow.ui
    windows/flightdetailswindow.ui
    windows/flightlistwindow.ui
    windows/metarswindow.ui
    windows/settingswindow.ui
    windows/vatsinatorwindow.ui
)

if (${vatsinator_UI_IMPLEMENTATION} MATCHES "widgets")
  vatsinator_subdirectory_forms (ui "${widgets_FORMS}")
endif (${vatsinator_UI_IMPLEMENTATION} MATCHES "widgets")

set (ui_QRCS
    resources/files.qrc
    resources/imgs.qrc
    resources/shaders.qrc
)

vatsinator_subdirectory_resources (ui "${ui_QRCS}")

# icon for Windows
if (WIN32)
  vatsinator_subdirectory_sources (ui "${PROJECT_SOURCE_DIR}/dist/vatsinator.rc")
endif (WIN32)

vatsinator_end_subdirectory (ui)