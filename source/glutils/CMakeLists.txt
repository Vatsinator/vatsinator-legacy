vatsinator_start_subdirectory (glutils)

set (glutils_SRCS
	VertexBufferObject.cpp
	
	glExtensions.cpp
)

vatsinator_subdirectory_sources (glutils "${glutils_SRCS}")


vatsinator_end_subdirectory (glutils)