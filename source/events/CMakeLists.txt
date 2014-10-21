vatsinator_start_subdirectory (events)

set (events_SRCS
    decisionevent.cpp
    mouselonlatevent.cpp
    notificationevent.cpp
    types.cpp
)

vatsinator_subdirectory_sources (events "${events_SRCS}")

vatsinator_end_subdirectory (events)