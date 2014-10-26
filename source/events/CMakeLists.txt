vatsinator_start_subdirectory (events)

set (events_SRCS
  decisionevent.cpp
  mouselonlatevent.cpp
  notificationevent.cpp
  types.cpp
)
vatsinator_subdirectory_sources (events "${events_SRCS}")

set (events_HEADERS
  decisionevent.h
  mouselonlatevent.h
  notificationevent.h
  types.h
)
vatsinator_subdirectory_headers (events "${events_HEADERS}")

vatsinator_end_subdirectory (events)