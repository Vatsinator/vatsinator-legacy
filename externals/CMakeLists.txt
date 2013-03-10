# Use this directory to add any external libs, sources, packages, etc.

if (WIN32)

    # QuaZIP should be built only on win32 platform to generate
    # the dll. On Linux we use FindQuaZip.
    add_subdirectory (QuaZIP)
    
    # Export dll
    set (QUAZIP_SOURCE_DIR ${QUAZIP_SOURCE_DIR} PARENT_SCOPE)
    set (QUAZIP_LIBRARY_RELEASE ${QUAZIP_LIBRARY_RELEASE} PARENT_SCOPE)
    set (QUAZIP_LIBRARIES ${QUAZIP_LIBRARIES} PARENT_SCOPE)
    
elseif (LINUX)

    find_package (ZLIB REQUIRED)
    include_directories (${ZLIB_INCLUDE_DIRS}})
    
endif (WIN32)

