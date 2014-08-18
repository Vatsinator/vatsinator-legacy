vatsinator_start_subdirectory (glutils)

set (glutils_SRCS
    glextensions.cpp
    glresourcemanager.cpp
    texture.cpp
)

vatsinator_subdirectory_sources (glutils "${glutils_SRCS}")


vatsinator_end_subdirectory (glutils)