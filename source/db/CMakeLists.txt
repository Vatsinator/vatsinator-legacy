vatsinator_start_subdirectory (db)

set (db_SRCS
	AirportsDatabase.cpp
	FirsDatabase.cpp
	WorldMap.cpp
)

vatsinator_subdirectory_sources (db "${db_SRCS}")

vatsinator_end_subdirectory (db)

