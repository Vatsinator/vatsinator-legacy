vatsinator_start_subdirectory (modules)

set (modules_SRCS
    modulemanager.cpp
    vatbookhandler.cpp
    
    models/bookedatctablemodel.cpp
    
    vatbook/bookedcontroller.cpp
)

vatsinator_subdirectory_sources (modules "${modules_SRCS}")

vatsinator_end_subdirectory (modules)

add_dependencies (modules ui)
