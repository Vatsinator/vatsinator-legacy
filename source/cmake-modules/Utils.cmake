if (NOT QT_QMAKE_EXECUTABLE)
  get_target_property (QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
endif ()

macro (get_platform_qpa_plugin var)
  if (NOT QT_INSTALL_PLUGINS)
    exec_program (${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_PLUGINS"
      OUTPUT_VARIABLE QT_INSTALL_PLUGINS)
  endif ()
  
  if (APPLE)
    set (${var} ${QT_INSTALL_PLUGINS}/platforms/libqcocoa.dylib)
  elseif (WIN32 AND MSVC)
    set (${var} ${QT_INSTALL_PLUGINS}/platforms/qwindows.dll)
  endif ()
endmacro ()

macro (get_target_location var target)
  if (NOT APPLE)
    cmake_policy (PUSH)
    cmake_policy (SET CMP0026 OLD)
    get_target_property (${var} ${target} LOCATION)
    cmake_policy (POP)
  else ()
    set (${var} ${target}.app)
  endif ()
endmacro ()


