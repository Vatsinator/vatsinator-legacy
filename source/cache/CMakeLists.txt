vatsinator_start_subdirectory (cache)


set (cache_SRCS
    cachefile.cpp
)

vatsinator_subdirectory_sources (cache "${cache_SRCS}")

set (cache_MOC_SRCS
    cachefile.h
)

vatsinator_subdirectory_mocs (cache "${cache_MOC_SRCS}")


vatsinator_end_subdirectory (cache)

