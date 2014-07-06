vatsinator_start_subdirectory (events)

set (events_SRCS
    mapevent.cpp
    mouselonlatevent.cpp
    types.cpp
)

vatsinator_subdirectory_sources (events "${events_SRCS}")

vatsinator_end_subdirectory (events)