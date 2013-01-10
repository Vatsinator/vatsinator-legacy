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
  
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/vatsinator.desktop.in
      ${DESKTOP_ENTRY})
  
  install (FILES ${DESKTOP_ENTRY}
    DESTINATION share/applications
  )
  
  # install icons
  add_subdirectory (linux-icons)
  
endif (LINUX)
