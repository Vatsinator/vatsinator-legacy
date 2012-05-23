vatsinator_start_subdirectory (vdebug)

set (debug_SRCS
	debugwindow.cpp
	
	glerrors.cpp
)

vatsinator_subdirectory_sources (vdebug "${debug_SRCS}")

set (debug_MOC_SRCS
	debugwindow.h
)

vatsinator_subdirectory_mocs (vdebug "${debug_MOC_SRCS}")

set (debug_FORMS
	resources/debugwindow.ui
)

vatsinator_subdirectory_forms (vdebug "${debug_FORMS}")

vatsinator_end_subdirectory (vdebug)
