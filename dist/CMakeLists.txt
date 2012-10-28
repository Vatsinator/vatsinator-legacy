if (LINUX)
  
  if (WITH_DEBIAN)
      configure_file (${CMAKE_CURRENT_SOURCE_DIR}/maketarball.sh.in
          ${CMAKE_CURRENT_SOURCE_DIR}/maketarball.sh @ONLY)
  endif (WITH_DEBIAN)
  
  if (WITH_RPM)
      set (ENV{LC_TIME} "en_US.UTF-8")
      execute_process(COMMAND date "+%a %b %d %Y"
          OUTPUT_VARIABLE vatsinator_RPM_DATE OUTPUT_STRIP_TRAILING_WHITESPACE)
      configure_file (${CMAKE_CURRENT_SOURCE_DIR}/vatsinator.spec.in
          ${CMAKE_CURRENT_SOURCE_DIR}/vatsinator.spec @ONLY)
  endif (WITH_RPM)
  
  
  # create desktop entry file
  if (NOT DESKTOP_ENTRY)
      set (DESKTOP_ENTRY ${CMAKE_SOURCE_DIR}/dist/vatsinator.desktop)
  endif (NOT DESKTOP_ENTRY)
  
  set (DESKTOP_SH_FILE ${CMAKE_SOURCE_DIR}/dist/vatsinator-desktop.sh)
  
  add_custom_command (OUTPUT ${DESKTOP_ENTRY}
      COMMAND touch ${DESKTOP_ENTRY}
      COMMAND sh ${DESKTOP_SH_FILE} ${CMAKE_INSTALL_PREFIX} > ${DESKTOP_ENTRY}
      DEPENDS ${DESKTOP_SH_FILE}
      COMMENT "Generating desktop entry file"
  )
  
  add_custom_target (DesktopEntryFile ALL
      DEPENDS ${DESKTOP_ENTRY}
  )
  
  install (FILES ${DESKTOP_ENTRY}
    DESTINATION share/applications
  )
  
endif (LINUX)
