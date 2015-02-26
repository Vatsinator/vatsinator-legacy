set (events_SRCS
  decisionevent.cpp
  mouselonlatevent.cpp
  notificationevent.cpp
  types.cpp
)

set (events_HEADERS
  decisionevent.h
  mouselonlatevent.h
  notificationevent.h
  types.h
)

add_library (events OBJECT
  ${events_SRCS}
  ${events_HEADERS}
)
