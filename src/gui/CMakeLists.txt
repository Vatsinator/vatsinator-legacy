find_package (Qt5Core REQUIRED)
find_package (Qt5Network REQUIRED)
find_package (Qt5Gui REQUIRED)

set (gui_SOURCES
    airportitem.cpp
    firarea.cpp
    firitem.cpp
    flightitem.cpp
    maparea.cpp
    mapdrawable.cpp
    mapitem.cpp
    maprenderer.cpp
    mapscene.cpp
    modelmatcher.cpp
    pixmapprovider.cpp
    tmaarea.cpp
    worldpainter.cpp
    worldtransform.cpp
    worldviewport.cpp
)

set (gui_HEADERS
    airportitem.h
    firarea.h
    firitem.h
    flightitem.h
    mapaddon.h
    maparea.h
    mapdrawable.h
    mapdrawer.h
    mapdrawerplugin.h
    mapitem.h
    maprenderer.h
    mapscene.h
    modelmatcher.h
    pixmapprovider.h
    tmaarea.h
    worldpainter.h
    worldtransform.h
    worldviewport.h
    
    gui.h
    guiexport.h
)

set (gui_QRCS
    resources/pixmaps.qrc
)

add_library (vtrgui SHARED ${gui_SOURCES} ${gui_HEADERS} ${gui_QRCS})
target_link_libraries (vtrgui vtrcore Qt5::Gui)
set_target_properties (vtrgui PROPERTIES CXX_VISIBILITY_PRESET hidden)
install (TARGETS vtrgui
    EXPORT vtrgui
    RUNTIME DESTINATION ${vatsinator_BINARY_TARGET_DIR}
    LIBRARY DESTINATION ${vatsinator_LIBRARY_TARGET_DIR}
    ARCHIVE DESTINATION ${vatsinator_ARCHIVE_TARGET_DIR} CONFIGURATIONS Debug
)

add_subdirectory (resources)
