file (GLOB TRANSLATION_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}-*.ts")

file (GLOB LANGUAGE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.language")

vatsinator_install ("${LANGUAGE_FILES}" "/translations")

qt4_add_translation (vatsinator_TRANSLATIONS ${TRANSLATION_SOURCES})

add_custom_target (i18n DEPENDS ${vatsinator_TRANSLATIONS})

vatsinator_install ("${vatsinator_TRANSLATIONS}" "/translations")

