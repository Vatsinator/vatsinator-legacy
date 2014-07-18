vatsinator_start_subdirectory (events)

set (events_SRCS
    mapevent.cpp
    mouselonlatevent.cpp
    notificationevent.cpp
    requestfinishedevent.cpp
)

vatsinator_subdirectory_sources (events "${events_SRCS}")

vatsinator_end_subdirectory (events)