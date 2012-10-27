macro (vatsinator_install INSTALL_FILES INSTALL_DIR)
    if (LINUX)
        install (FILES ${INSTALL_FILES}
                 DESTINATION share/vatsinator${INSTALL_DIR})
    elseif (WIN32)
        install (FILES ${INSTALL_FILES}
                 DESTINATION .${INSTALL_DIR})
    elseif (APPLE)
        string (REPLACE "/" "_" INSTALL_FILES ${INSTALL_FILES})
        foreach (f ${INSTALL_FILES})
          set (vatsinator_BUNDLE_RESOURCES${INSTALL_DIR}
               ${vatsinator_BUNDLE_RESOURCES${INSTALL_DIR}}  ${CMAKE_CURRENT_SOURCE_DIR}/${f})
        endforeach (f ${INSTALL_FILES})
    endif (LINUX)
endmacro (vatsinator_install)
