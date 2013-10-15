vatsinator_start_subdirectory (glutils)

set (glutils_SRCS
    glresourcemanager.cpp
    vertexbufferobject.cpp
    
    glextensions.cpp
)

vatsinator_subdirectory_sources (glutils "${glutils_SRCS}")


vatsinator_end_subdirectory (glutils)