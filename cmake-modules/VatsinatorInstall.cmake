macro (vatsinator_install INSTALL_FILES INSTALL_DIR)
    if (LINUX)
        install (FILES ${INSTALL_FILES}
                 DESTINATION share/vatsinator${INSTALL_DIR})
    elseif (WIN32)
        install (FILES ${INSTALL_FILES}
                 DESTINATION .${INSTALL_DIR})
    elseif (APPLE)
        string (REPLACE "/" "_" DIR_NAME "${INSTALL_DIR}")
        foreach (f ${INSTALL_FILES})
          set (vatsinator_BUNDLE_RESOURCES${DIR_NAME}
               ${vatsinator_BUNDLE_RESOURCES${DIR_NAME}}  ${CMAKE_CURRENT_SOURCE_DIR}/${f})
          if (NOT ${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
            set (vatsinator_BUNDLE_RESOURCES${DIR_NAME}
              ${vatsinator_BUNDLE_RESOURCES${DIR_NAME}} PARENT_SCOPE)
          endif (NOT ${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
        endforeach (f ${INSTALL_FILES})
    endif (LINUX)
endmacro (vatsinator_install)
