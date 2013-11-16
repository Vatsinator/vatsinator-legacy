# generate macros
add_subdirectory (pages)

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
    userinterface.cpp
    
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
    
    pages/abstractsettingspage.cpp
    pages/colorspage.cpp
    pages/miscellaneouspage.cpp
    pages/networkpage.cpp
    pages/viewpage.cpp
    
    views/airporttableview.cpp
    views/controllertableview.cpp
    views/flighttableview.cpp
    
    widgets/mapwidget.cpp
    widgets/replaceablewidget.cpp
    widgets/scrolllabel.cpp
    
    windows/aboutwindow.cpp
    windows/airportdetailswindow.cpp
    windows/atcdetailswindow.cpp
    windows/atclistwindow.cpp
    windows/basewindow.cpp
    windows/firdetailswindow.cpp
    windows/flightdetailswindow.cpp
    windows/flightlistwindow.cpp
    windows/metarswindow.cpp
    windows/settingswindow.cpp
    windows/vatsinatorwindow.cpp
)

vatsinator_subdirectory_sources (ui "${ui_SRCS}")

set (ui_MOC_SRCS
    userinterface.h
    
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
    
    pages/abstractsettingspage.h
    pages/colorspage.h
    pages/miscellaneouspage.h
    pages/networkpage.h
    pages/viewpage.h
    
    views/airporttableview.h
    views/controllertableview.h
    views/flighttableview.h
    
    widgets/mapwidget.h
    widgets/replaceablewidget.h
    widgets/scrolllabel.h
    
    windows/aboutwindow.h
    windows/airportdetailswindow.h
    windows/atcdetailswindow.h
    windows/atclistwindow.h
    windows/basewindow.h
    windows/firdetailswindow.h
    windows/flightdetailswindow.h
    windows/flightlistwindow.h
    windows/metarswindow.h
    windows/settingswindow.h
    windows/vatsinatorwindow.h
)

vatsinator_subdirectory_mocs (ui "${ui_MOC_SRCS}")

set (ui_FORMS
    dialogs/apprestartdialog.ui
    dialogs/letsendstatsdialog.ui
    dialogs/newversiondialog.ui
    dialogs/vatsimmessagedialog.ui
    
    pages/colorspage.ui
    pages/miscellaneouspage.ui
    pages/networkpage.ui
    pages/viewpage.ui

    windows/aboutwindow.ui
    windows/airportdetailswindow.ui
    windows/atcdetailswindow.ui
    windows/atclistwindow.ui
    windows/firdetailswindow.ui
    windows/flightdetailswindow.ui
    windows/flightlistwindow.ui
    windows/metarswindow.ui
    windows/settingswindow.ui
    windows/vatsinatorwindow.ui
)

vatsinator_subdirectory_forms (ui "${ui_FORMS}")

set (ui_QRCS
    resources/imgs.qrc
)

vatsinator_subdirectory_resources (ui "${ui_QRCS}")

# icon for Windows
if (WIN32)
      vatsinator_subdirectory_sources (ui "${PROJECT_SOURCE_DIR}/dist/vatsinator.rc")
endif (WIN32)

vatsinator_end_subdirectory (ui)

add_dependencies (ui debugging)
