vatsinator_start_subdirectory (debugging)

set (debugging_SRCS
    debugwindow.cpp
    memorytracker.cpp
    
    glerrors.cpp
)

vatsinator_subdirectory_sources (debugging "${debugging_SRCS}")

set (debugging_MOC_SRCS
    debugwindow.h
)

vatsinator_subdirectory_mocs (debugging "${debugging_MOC_SRCS}")

set (debugging_FORMS
    resources/debugwindow.ui
)

vatsinator_subdirectory_forms (debugging "${debugging_FORMS}")

vatsinator_end_subdirectory (debugging)
