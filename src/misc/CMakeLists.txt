find_package (Qt5 5.7 REQUIRED COMPONENTS Core Network Gui)

set (misc_SOURCES
    airlinelogoreply.cpp
    atcbooking.cpp
    atcbookinglistmodel.cpp
    atcbookingreply.cpp
    atclistmodel.cpp
    clientlistmodel.cpp
    dataprovider.cpp
    filecache.cpp
    filedownloader.cpp
    filereader.cpp
    flightlistmodel.cpp
    notam.cpp
    notamlistmodel.cpp
    notamreply.cpp
    option.cpp
    pluginfinder.cpp
    resourcefile.cpp
)

set (misc_HEADERS
    airlinelogoprovider.h
    airlinelogoreply.h
    atcbooking.h
    atcbookinglistmodel.h
    atcbookingprovider.h
    atcbookingreply.h
    atclistmodel.h
    clientlistmodel.h
    dataprovider.h
    filecache.h
    filedownloader.h
    filereader.h
    flightlistmodel.h
    notam.h
    notamlistmodel.h
    notamprovider.h
    notamreply.h
    option.h
    pluginfinder.h
    resourcefile.h
    servertrackerprovider.h
    
    misc.h
    roles.h
)

add_library (v8rmisc SHARED ${misc_SOURCES} ${misc_HEADERS})
target_link_libraries (v8rmisc v8rcore Qt5::Gui)
set_target_properties (v8rmisc PROPERTIES CXX_VISIBILITY_PRESET hidden)

include (GenerateExportHeader)
generate_export_header(v8rmisc)

install (TARGETS v8rmisc
    EXPORT v8rmisc
    RUNTIME DESTINATION ${vatsinator_BINARY_TARGET_DIR}
    LIBRARY DESTINATION ${vatsinator_LIBRARY_TARGET_DIR}
    ARCHIVE DESTINATION ${vatsinator_ARCHIVE_TARGET_DIR} CONFIGURATIONS Debug
)
