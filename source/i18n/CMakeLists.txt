file (GLOB TRANSLATION_SOURCES
    "${CMAKE_PROJECT_NAME}-*.ts")

file (GLOB LANGUAGE_FILES
    "*.language")

vatsinator_install ("${LANGUAGE_FILES}" "/translations")

qt4_add_translation (vatsinator_TRANSLATIONS ${TRANSLATION_SOURCES})

add_custom_target (i18n DEPENDS ${vatsinator_TRANSLATIONS})

#vatsinator_install ("${vatsinator_TRANSLATIONS}" "/translations")
add_custom_command (
  TARGET i18n POST_BUILD
  COMMAND cp ARGS -R ${vatsinator_TRANSLATIONS}
    ${CMAKE_BINARY_DIR}/vatsinator.resources/translations/
  )
