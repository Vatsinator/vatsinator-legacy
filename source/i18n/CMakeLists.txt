file (GLOB TRANSLATION_SOURCES
    "${CMAKE_PROJECT_NAME}-*.ts")

file (GLOB LANGUAGE_FILES
    "*.language")

vatsinator_install ("${LANGUAGE_FILES}" "/translations")

qt4_add_translation (vatsinator_TRANSLATIONS ${TRANSLATION_SOURCES})

if (APPLE)
  file (GLOB MAC_TRANSLATION_SOURCES
        "mac-app-menu-*.ts")
  qt4_add_translation (vatsinator_MAC_TRANSLATIONS ${MAC_TRANSLATION_SOURCES})
  set (vatsinator_TRANSLATIONS ${vatsinator_TRANSLATIONS} ${vatsinator_MAC_TRANSLATIONS})
endif (APPLE)

add_custom_target (i18n DEPENDS ${vatsinator_TRANSLATIONS})

if (NOT APPLE)
  vatsinator_install ("${vatsinator_TRANSLATIONS}" "/translations")
else (NOT APPLE)

  # vatisnator_install macro works only for existing files
  add_custom_command (
    TARGET i18n POST_BUILD
    COMMAND cp ARGS -R ${vatsinator_TRANSLATIONS}
      ${CMAKE_BINARY_DIR}/vatsinator.resources/translations/
    COMMENT "Copying files to the bundle..."
    )
  
  foreach (LANG ${LANGUAGE_FILES})
    string (REGEX
      REPLACE "^.*/([a-z]+)\\.language$"
      "\\1"
      lang_symbol ${LANG}
    )
    
    add_custom_command (
      TARGET i18n POST_BUILD
      COMMAND mkdir ARGS -p
        ${CMAKE_BINARY_DIR}/vatsinator.resources/${lang_symbol}.lproj
      COMMENT "Enabling MacOS menu translations for ${lang_symbol}..."
    )
    
    configure_file (
      ${CMAKE_CURRENT_SOURCE_DIR}/locversion.plist.in
      ${CMAKE_BINARY_DIR}/vatsinator.resources/${lang_symbol}.lproj/locversion.plist
    )
    
  endforeach (LANG)
endif (NOT APPLE)
