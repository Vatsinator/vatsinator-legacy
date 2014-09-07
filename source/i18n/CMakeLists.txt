find_package (Qt5LinguistTools REQUIRED) # qt5_add_translation

file (GLOB LANGUAGE_FILES
    "*.language")
vatsinator_install ("${LANGUAGE_FILES}" "/translations")

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
  exec_program (${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_TRANSLATIONS"
      OUTPUT_VARIABLE QT_TRANSLATIONS_DIR)
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

if (NOT APPLE)
  vatsinator_install ("${vatsinator_TRANSLATIONS}" "/translations")
  vatsinator_install ("${qt_TRANSLATIONS}" "/translations")
else (NOT APPLE)

  # vatisnator_install macro works only for existing files
  add_custom_command (
    TARGET i18n POST_BUILD
    COMMAND cp ARGS -R ${vatsinator_TRANSLATIONS} ${qt_TRANSLATIONS}
      ${CMAKE_BINARY_DIR}/vatsinator.resources/translations/
    COMMENT "Copying files to the bundle..."
    )
  
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
    
  endforeach (LANG)
endif (NOT APPLE)
