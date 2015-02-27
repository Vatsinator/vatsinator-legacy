find_package (Qt5LinguistTools REQUIRED) # qt5_add_translation

file (GLOB LANGUAGE_FILES
    "*.language")
install (FILES ${LANGUAGE_FILES} DESTINATION ${vatsinator_INSTALL_DIR}/translations)

set (vatsinator_LANGUAGES "")

# get all languages
foreach (LANG ${LANGUAGE_FILES})
  string (REGEX
    REPLACE "^.*/(.+)\\.language$"
    "\\1"
    lang_symbol ${LANG}
  )
  set (vatsinator_LANGUAGES ${vatsinator_LANGUAGES} ${lang_symbol})
endforeach (LANG)

if (NOT QT_TRANSLATIONS_DIR)
  query_qmake (QT_TRANSLATIONS_DIR QT_INSTALL_TRANSLATIONS)
endif (NOT QT_TRANSLATIONS_DIR)

# gather .ts files and qt translations
set (TRANSLATION_SOURCES "")
set (qt_TRANSLATIONS "")
foreach (LANG ${vatsinator_LANGUAGES})
  set (TS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}-${LANG}.ts")
  set (TRANSLATION_SOURCES ${TRANSLATION_SOURCES} ${TS_FILE})
  
  if (NOT "en" STREQUAL "${LANG}")
    set (QM_FILE "${QT_TRANSLATIONS_DIR}/qt_${LANG}.qm")
    if (NOT EXISTS ${QM_FILE})
      string (SUBSTRING "${LANG}" 0 2 LANG_NO_REGION)
      set (QM_FILE "${QT_TRANSLATIONS_DIR}/qt_${LANG_NO_REGION}.qm")
    endif (NOT EXISTS ${QM_FILE})
    
    if (EXISTS ${QM_FILE})
      set (qt_TRANSLATIONS ${qt_TRANSLATIONS} ${QM_FILE})
    else (EXISTS ${QM_FILE})
      message (WARNING "No Qt translation for ${LANG}!")
    endif (EXISTS ${QM_FILE})
  endif (NOT "en" STREQUAL "${LANG}")
endforeach (LANG)

qt5_add_translation (vatsinator_TRANSLATIONS ${TRANSLATION_SOURCES})

add_custom_target (i18n DEPENDS ${vatsinator_TRANSLATIONS} ${qt_TRANSLATIONS})
install (FILES ${vatsinator_TRANSLATIONS} DESTINATION ${vatsinator_INSTALL_DIR}/translations)
install (FILES ${qt_TRANSLATIONS} DESTINATION ${vatsinator_INSTALL_DIR}/translations)

if (APPLE)  
  foreach (LANG ${vatsinator_LANGUAGES})
    add_custom_command (
      TARGET i18n POST_BUILD
      COMMAND mkdir ARGS -p
        ${CMAKE_BINARY_DIR}/vatsinator.resources/${LANG}.lproj
      COMMENT "Enabling MacOS menu translations for ${LANG}..."
    )
    
    configure_file (
      ${CMAKE_CURRENT_SOURCE_DIR}/locversion.plist.in
      ${CMAKE_BINARY_DIR}/vatsinator.resources/${LANG}.lproj/locversion.plist
    )

    install (
      FILES
        "${CMAKE_BINARY_DIR}/vatsinator.resources/${LANG}.lproj/locversion.plist"
      DESTINATION
        ${vatsinator_INSTALL_DIR}/${LANG}.lproj
    )
    
  endforeach (LANG)
endif ()
