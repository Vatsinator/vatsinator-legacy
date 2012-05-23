vatsinator_start_subdirectory (db)

set (db_SRCS
	airportsdatabase.cpp
	firsdatabase.cpp
	worldmap.cpp
)

vatsinator_subdirectory_sources (db "${db_SRCS}")

vatsinator_end_subdirectory (db)

