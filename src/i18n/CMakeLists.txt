include (QtUtils)

if (NOT QT_TRANSLATIONS_DIR)
    query_qmake (QT_TRANSLATIONS_DIR QT_INSTALL_TRANSLATIONS)
endif ()

# Looks for qt translation
function (add_qt_qm_file output prefix lang_code)
    set (qm_file "${QT_TRANSLATIONS_DIR}/${prefix}_${lang_code}.qm")
    if (NOT EXISTS ${qm_file})
        string (SUBSTRING "${lang_code}" 0 2 lang_code_short) # language code without region
        set (qm_file "${QT_TRANSLATIONS_DIR}/${prefix}_${lang_code_short}.qm")
    endif ()

    if (EXISTS ${qm_file})
        set (${output} ${${output}} ${qm_file} PARENT_SCOPE)
    endif ()
endfunction ()


find_package (Qt5LinguistTools REQUIRED) # for qt5_add_translation

file (GLOB translation_sources "*.ts")
qt5_add_translation (translations ${translation_sources})
add_custom_target (i18n
    DEPENDS ${translations}
    SOURCES ${translation_sources} locversion.plist.in
)

install (FILES ${translations} DESTINATION ${vatsinator_RESOURCES_TARGET_DIR}/translations)

foreach (file ${translation_sources})
    string (REGEX
        REPLACE "^.*/.[^_]+_(.+)\\.ts$"
        "\\1"
        language_code ${file}
    )

    if (NOT "${language_code}" STREQUAL "en")
        add_qt_qm_file (qt_qms "qt" "${language_code}")
        add_qt_qm_file (qt_qms "qtbase" "${language_code}")
    endif ()

    list (APPEND vatsinator_LANGUAGES ${language_code})
endforeach ()

install (FILES ${qt_qms} DESTINATION ${vatsinator_RESOURCES_TARGET_DIR}/translations)

if (APPLE)
    foreach (lang ${vatsinator_LANGUAGES})
        add_custom_command (
            TARGET i18n POST_BUILD
            COMMAND mkdir ARGS -p
                ${CMAKE_BINARY_DIR}/vatsinator.resources/${lang}.lproj
            COMMENT "Enabling MacOS menu translations for ${lang}..."
        )

        configure_file (
            ${CMAKE_CURRENT_SOURCE_DIR}/locversion.plist.in
            ${CMAKE_BINARY_DIR}/vatsinator.resources/${lang}.lproj/locversion.plist
        )

        install (FILES "${CMAKE_BINARY_DIR}/vatsinator.resources/${lang}.lproj/locversion.plist"
            DESTINATION ${vatsinator_RESOURCES_TARGET_DIR}/${lang}.lproj
        )
    endforeach ()
endif ()
