if (NOT QT_QMAKE_EXECUTABLE)
    get_target_property (QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
endif ()



macro (query_qmake var property)
    execute_process (
        COMMAND ${QT_QMAKE_EXECUTABLE} -query ${property}
        OUTPUT_VARIABLE ${var}
    )
    string (STRIP ${${var}} ${var})
endmacro ()