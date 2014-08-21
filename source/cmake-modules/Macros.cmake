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


macro (vatsinator_subdirectory_mocs SUBDIR MOC_SRCS)
  if (${QT_VERSION} VERSION_LESS 5.0)
    if (NOT "${MOC_SRCS}" STREQUAL "")
      qt4_wrap_cpp (vatsinator_${SUBDIR}_MOC_FILES ${MOC_SRCS})
    endif (NOT "${MOC_SRCS}" STREQUAL "")
  endif (${QT_VERSION} VERSION_LESS 5.0)
endmacro (vatsinator_subdirectory_mocs)


macro (vatsinator_subdirectory_forms SUBDIR FORMS)
  if (NOT "${FORMS}" STREQUAL "")
    if (${QT_VERSION} VERSION_LESS 5.0)
      qt4_wrap_ui (vatsinator_${SUBDIR}_FORMS_FILES ${FORMS})
    else (${QT_VERSION} VERSION_LESS 5.0)
      qt5_wrap_ui (vatsinator_${SUBDIR}_FORMS_FILES ${FORMS})
    endif (${QT_VERSION} VERSION_LESS 5.0)
  endif (NOT "${FORMS}" STREQUAL "")
endmacro (vatsinator_subdirectory_forms)


macro (vatsinator_subdirectory_resources SUBDIR QRCS)
  if (NOT "${QRCS}" STREQUAL "")
    if (${QT_VERSION} VERSION_LESS 5.0)
      qt4_add_resources (vatsinator_${SUBDIR}_RESOURCES ${QRCS})
    else (${QT_VERSION} VERSION_LESS 5.0)
      qt5_add_resources (vatsinator_${SUBDIR}_RESOURCES ${QRCS})
    endif (${QT_VERSION} VERSION_LESS 5.0)
  endif (NOT "${QRCS}" STREQUAL "")
endmacro (vatsinator_subdirectory_resources)


macro (vatsinator_end_subdirectory SUBDIR)
  add_library (${SUBDIR} STATIC
    ${vatsinator_${SUBDIR}_SOURCES}
    ${vatsinator_${SUBDIR}_FORMS_FILES}
    ${vatsinator_${SUBDIR}_RESOURCES}
  )
endmacro (vatsinator_end_subdirectory)
