macro (vatsinator_install INSTALL_FILES INSTALL_DIR)
    if (LINUX)
        install (FILES ${INSTALL_FILES}
                 DESTINATION share/vatsinator${INSTALL_DIR})
    elseif (WIN32)
        install (FILES ${INSTALL_FILES}
                 DESTINATION .${INSTALL_DIR})
    elseif (APPLE)
        install (FILES ${INSTALL_FILES}
                DESTINATION vatsinator.app/Contents/Resources${INSTALL_DIR})
    endif (LINUX)
endmacro (vatsinator_install)
