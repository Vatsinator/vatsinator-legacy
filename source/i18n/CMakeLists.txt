if (NOT QT_TRANSLATIONS_DIR)
    query_qmake (QT_TRANSLATIONS_DIR QT_INSTALL_TRANSLATIONS)
endif ()

function (add_qt_tr_file output prefix lang_code)
    set (qm_file "${QT_TRANSLATIONS_DIR}/${prefix}_${lang_code}.qm")
    if (NOT EXISTS ${qm_file})
        string (SUBSTRING "${lang_code}" 0 2 lang_code) # language code without region
        set (qm_file "${QT_TRANSLATIONS_DIR}/${prefix}_${lang_code}.qm")
    endif ()
    
    if (EXISTS ${qm_file})
        set (${output} ${${output}} ${qm_file} PARENT_SCOPE)
    endif ()
endfunction ()



find_package (Qt5LinguistTools REQUIRED) # for qt5_add_translation

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

# gather .ts files and qt translations
set (TRANSLATION_SOURCES "")
set (qt_TRANSLATIONS "")
foreach (LANG ${vatsinator_LANGUAGES})
    set (TS_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}-${LANG}.ts")
    set (TRANSLATION_SOURCES ${TRANSLATION_SOURCES} ${TS_FILE})
    
    if (NOT "en" STREQUAL "${LANG}")
        add_qt_tr_file(qt_TRANSLATIONS qt ${LANG})
        add_qt_tr_file(qt_TRANSLATIONS qtbase ${LANG})
    endif ()
endforeach ()

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
