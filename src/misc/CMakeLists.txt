find_package (Qt5Gui REQUIRED)

set (misc_SOURCES
    airlinelogoreply.cpp
    atcbooking.cpp
    atcbookinglistmodel.cpp
    atcbookingreply.cpp
    atclistmodel.cpp
    clientlistmodel.cpp
    filecache.cpp
    flightlistmodel.cpp
    metarlistmodel.cpp
    notam.cpp
    notamlistmodel.cpp
    notamreply.cpp
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
    filecache.h
    flightlistmodel.h
    metarlistmodel.h
    notam.h
    notamlistmodel.h
    notamprovider.h
    notamreply.h
    
    misc.h
    miscexport.h
    roles.h
)

add_library (vtrmisc SHARED ${misc_SOURCES} ${misc_HEADERS})
target_link_libraries (vtrmisc vtrcore Qt5::Gui)
set_target_properties (vtrmisc PROPERTIES CXX_VISIBILITY_PRESET hidden)
install (TARGETS vtrmisc
    EXPORT vtrmisc
    RUNTIME DESTINATION bin CONFIGURATIONS Debug Release
    LIBRARY DESTINATION lib/vatsinator CONFIGURATIONS Debug
    ARCHIVE DESTINATION lib CONFIGURATIONS Debug
)
