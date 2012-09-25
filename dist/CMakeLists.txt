if (WITH_DEBIAN)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/maketarball.sh.in
        ${CMAKE_CURRENT_SOURCE_DIR}/maketarball.sh @ONLY)
endif (WITH_DEBIAN)
