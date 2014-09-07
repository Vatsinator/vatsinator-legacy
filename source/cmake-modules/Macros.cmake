# there are some macros defined to provide easier build of each subdirectory's lib

macro (vatsinator_start_subdirectory SUBDIR)
  set (vatsinator_${SUBDIR}_SOURCES "")
  set (vatsinator_${SUBDIR}_MOC_FILES "")
  set (vatsinator_${SUBDIR}_FORMS_FILES "")
  set (vatsinator_${SUBDIR}_RESOURCES "")
endmacro (vatsinator_start_subdirectory)


macro (vatsinator_subdirectory_sources SUBDIR SRCS)
  if (NOT "${SRCS}" STREQUAL "")
    set (vatsinator_${SUBDIR}_SOURCES
         ${vatsinator_${SUBDIR}_SOURCES} ${SRCS})
  endif (NOT "${SRCS}" STREQUAL "")
endmacro (vatsinator_subdirectory_sources)


macro (vatsinator_subdirectory_forms SUBDIR FORMS)
  if (NOT "${FORMS}" STREQUAL "")
    qt5_wrap_ui (vatsinator_${SUBDIR}_FORMS_FILES ${FORMS})
  endif (NOT "${FORMS}" STREQUAL "")
endmacro (vatsinator_subdirectory_forms)


macro (vatsinator_subdirectory_resources SUBDIR QRCS)
  if (NOT "${QRCS}" STREQUAL "")
    qt5_add_resources (vatsinator_${SUBDIR}_RESOURCES ${QRCS})
  endif (NOT "${QRCS}" STREQUAL "")
endmacro (vatsinator_subdirectory_resources)


macro (vatsinator_end_subdirectory SUBDIR)
  add_library (${SUBDIR} STATIC
    ${vatsinator_${SUBDIR}_SOURCES}
    ${vatsinator_${SUBDIR}_FORMS_FILES}
    ${vatsinator_${SUBDIR}_RESOURCES}
  )
endmacro (vatsinator_end_subdirectory)
