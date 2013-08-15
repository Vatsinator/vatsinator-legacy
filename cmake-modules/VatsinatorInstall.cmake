macro (vatsinator_install INSTALL_FILES INSTALL_DIR)
    if (LINUX)
        install (FILES ${INSTALL_FILES}
                 DESTINATION share/vatsinator${INSTALL_DIR})
    elseif (WIN32)
        install (FILES ${INSTALL_FILES}
                 DESTINATION .${INSTALL_DIR})
    elseif (APPLE)
        file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/vatsinator.resources${INSTALL_DIR}")
        foreach (f ${INSTALL_FILES})
          configure_file (${f} "${CMAKE_BINARY_DIR}/vatsinator.resources${INSTALL_DIR}/"
                          COPYONLY)
        endforeach (f)
    endif (LINUX)
endmacro (vatsinator_install)
