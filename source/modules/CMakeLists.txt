vatsinator_start_subdirectory (modules)

set (modules_SRCS
	flighttracker.cpp
	modelsmatcher.cpp
	modulesmanager.cpp
)

vatsinator_subdirectory_sources (modules "${modules_SRCS}")

set (modules_MOC_SRCS
	flighttracker.h
	modulesmanager.h
)

vatsinator_subdirectory_mocs (modules "${modules_MOC_SRCS}")

vatsinator_end_subdirectory (modules)

add_dependencies (modules ui)
