find_package (Qt5Core)

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

macro (get_platform_qpa_plugin var)
    if (NOT QT_INSTALL_PLUGINS)
        query_qmake (QT_INSTALL_PLUGINS QT_INSTALL_PLUGINS)
    endif ()
    
    if (APPLE)
        set (${var} ${QT_INSTALL_PLUGINS}/platforms/libqcocoa.dylib)
    elseif (WIN32 AND MSVC)
        set (${var} ${QT_INSTALL_PLUGINS}/platforms/qwindows.dll)
    endif ()
endmacro ()
