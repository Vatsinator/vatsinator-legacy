include (QtUtils)

if (NOT QT_TRANSLATIONS_DIR)
    query_qmake (QT_TRANSLATIONS_DIR QT_INSTALL_TRANSLATIONS)
endif ()

# Looks for qt translation
function (add_qt_qm_file output prefix lang_code)
    set (qm_file "${QT_TRANSLATIONS_DIR}/${prefix}_${lang_code}.qm")
    if (NOT EXISTS ${qm_file})
        string (SUBSTRING "${lang_code}" 0 2 lang_code) # language code without region
        set (qm_file "${QT_TRANSLATIONS_DIR}/${prefix}_${lang_code}.qm")
    endif ()

    if (EXISTS ${qm_file})
        set (${output} ${${output}} ${qm_file} PARENT_SCOPE)
    else ()
        message (STATUS "No Qt translation for ${lang_code}")
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
endforeach ()

install (FILES ${qt_qms} DESTINATION ${vatsinator_RESOURCES_TARGET_DIR}/translations)
