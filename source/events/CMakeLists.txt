vatsinator_start_subdirectory (events)

set (events_SRCS
    mouselonlatevent.cpp
    notificationevent.cpp
)

vatsinator_subdirectory_sources (events "${events_SRCS}")

vatsinator_end_subdirectory (events)