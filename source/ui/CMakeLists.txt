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

if (NOT vatsinator_LICENSE)
    file (READ ${CMAKE_SOURCE_DIR}/COPYING vatsinator_LICENSE)
    string (REPLACE "<" "&lt;" vatsinator_LICENSE "${vatsinator_LICENSE}")
    string (REPLACE ">" "&gt;" vatsinator_LICENSE "${vatsinator_LICENSE}")
    string (REPLACE "\n" "<br>" vatsinator_LICENSE "${vatsinator_LICENSE}")
    string (REPLACE "\"" "\\\"" vatsinator_LICENSE "${vatsinator_LICENSE}")
endif (NOT vatsinator_LICENSE)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/license.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/license.h
)

set (ui_SRCS
    notifiable.cpp
    userinterface.cpp
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
    
    map/abstractanimation.cpp
    map/airportitem.cpp
    map/approachcircleitem.cpp
    map/firitem.cpp
    map/flightitem.cpp
    map/iconkeeper.cpp
    map/mapconfig.cpp
    map/mapitem.cpp
    map/mapscene.cpp
    map/mapstate.cpp
    map/moveanimation.cpp
    map/uiritem.cpp
    map/worldpolygon.cpp
    
    pages/mappage.cpp
    pages/miscellaneouspage.cpp
    pages/networkpage.cpp
    pages/viewpage.cpp
    
    views/airporttableview.cpp
    views/controllertableview.cpp
    views/delayedmodeltableview.cpp
    views/flighttableview.cpp
    
    widgets/flightprogresswidget.cpp
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


set (ui_MOC_SRCS
    userinterface.h
)

set (widgets_MOC_SRCS
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
    
    map/abstractanimation.h
    map/airportitem.h
    map/approachcircleitem.h
    map/firitem.h
    map/flightitem.h
    map/iconkeeper.h
    map/mapscene.h
    map/moveanimation.h
    map/uiritem.h
    
    pages/mappage.h
    pages/miscellaneouspage.h
    pages/networkpage.h
    pages/viewpage.h
    
    views/airporttableview.h
    views/controllertableview.h
    views/delayedmodeltableview.h
    views/flighttableview.h
    
    widgets/flightprogresswidget.h
    widgets/mapwidget.h
    widgets/replaceablewidget.h
    widgets/scrolllabel.h
    widgets/weatherforecastwidget.h
    
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

vatsinator_subdirectory_mocs (ui "${ui_MOC_SRCS}")
if (${vatsinator_UI_IMPLEMENTATION} MATCHES "widgets")
  vatsinator_subdirectory_mocs (ui "${widgets_MOC_SRCS}")
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
    resources/imgs.qrc
    resources/shaders.qrc
)

vatsinator_subdirectory_resources (ui "${ui_QRCS}")

# icon for Windows
if (${vatsinator_UI_IMPLEMENTATION} MATCHES "widgets" AND WIN32)
  vatsinator_subdirectory_sources (ui "${PROJECT_SOURCE_DIR}/dist/vatsinator.rc")
endif (${vatsinator_UI_IMPLEMENTATION} MATCHES "widgets" AND WIN32)

vatsinator_end_subdirectory (ui)