set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")
include (Macros)

set (CMAKE_AUTOMOC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)

# exec file
set (vatsinator_TARGET
  vatsinator
)

set (vatsinator_SRCS
  netconfig.cpp
  vatsinatorapplication.cpp
)

set (vatsinator_HEADERS
  netconfig.h
  singleton.h
  vatsinatorapplication.h
)

# icon for Windows
if (WIN32)
  set (vatsinator_SRCS ${vatsinator_SRCS}
      ${PROJECT_SOURCE_DIR}/dist/vatsinator.rc)
endif ()

set (vatsinator_SUBDIRS
    db
    events
    network
    plugins
    storage
    ui
    vatsimdata
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable (${vatsinator_TARGET} MACOSX_BUNDLE WIN32
    main.cpp
    ${vatsinator_SRCS}
    ${vatsinator_HEADERS}
)

set (vatsinator_QT_LIBS
    Qt5::Widgets
    Qt5::Network
    Qt5::OpenGL
)
      
if (LINUX)
    set (vatsinator_PREFIX "${CMAKE_INSTALL_PREFIX}/share/vatsinator/")
    
    set (vatsinator_LIBS
        ${vatsinator_QT_LIBS}
        ${OPENGL_LIBRARY}
        ${QUAZIP_LIBRARIES}
    )
elseif (APPLE)
    set (vatsinator_PREFIX "")

    set (vatsinator_LIBS
        ${vatsinator_QT_LIBS}
        ${OPENGL_LIBRARY}
        ${QUAZIP_LIBRARIES}
    )
else (WIN32)
    set (vatsinator_PREFIX "")

    set (vatsinator_LIBS
        ${vatsinator_QT_LIBS}
        Qt5::WinMain
        ${OPENGL_LIBRARY}
        ${QUAZIP_LIBRARIES}
    )
endif (LINUX)

if (NOT vatsinator_UI_IMPLEMENTATION)
  message (FATAL_ERROR "No valid user interface implementation specified")
  message (STATUS "Known user interface implementations: widgets")
endif ()

if (NOT VATSINATOR_ENABLE_GESTURES)
  # gestures suppport disables mouse wheel map zooming
  if (APPLE)
    set (VATSINATOR_ENABLE_GESTURES 1)
  endif()
endif ()

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/vatsinator-config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

foreach (DIR ${vatsinator_SUBDIRS})
   add_subdirectory (${DIR})
    add_dependencies (${vatsinator_TARGET}
        ${DIR}
    )
endforeach (DIR)

add_subdirectory (i18n)
add_dependencies (${vatsinator_TARGET} i18n)

if (APPLE)
    target_link_libraries (${vatsinator_TARGET}
        -Wl,-all_load ${vatsinator_SUBDIRS}
    )
else (APPLE)
  if (CMAKE_COMPILER_IS_GNUCC)
    target_link_libraries (${vatsinator_TARGET}
        -Wl,--start-group
        -Wl,--whole-archive
        ${vatsinator_SUBDIRS}
        -Wl,--no-whole-archive
        -Wl,--end-group
    )
  elseif (MSVC)
    target_link_libraries (${vatsinator_TARGET} ${vatsinator_SUBDIRS})
  endif (CMAKE_COMPILER_IS_GNUCC)
endif (APPLE)

target_link_libraries (${vatsinator_TARGET} ${vatsinator_LIBS})

if (LINUX)
  # Qt is always already deployed on Linux
  install (TARGETS ${vatsinator_TARGET} RUNTIME DESTINATION bin)
elseif (APPLE OR WIN32)
  install (TARGETS ${vatsinator_TARGET} DESTINATION .)

  exec_program (${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_BINS"
    OUTPUT_VARIABLE QT_INSTALL_LIBS)
  exec_program (${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_PLUGINS"
    OUTPUT_VARIABLE QT_INSTALL_PLUGINS)

  if (APPLE)
    set (QT_QPA_PLUGIN ${QT_INSTALL_PLUGINS}/platforms/libqcocoa.dylib)
    set (vatsinator_EXECUTABLE ${vatsinator_TARGET}.app)
  elseif (WIN32 AND MSVC)
    set (QT_QPA_PLUGIN ${QT_INSTALL_PLUGINS}/platforms/qwindows.dll)
    get_property (vatsinator_EXECUTABLE TARGET ${vatsinator_TARGET} PROPERTY LOCATION)
    get_filename_component (vatsinator_EXECUTABLE ${vatsinator_EXECUTABLE} NAME)
  endif ()

  set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
  include(InstallRequiredSystemLibraries)

  if (NOT QUAZIP_LIBRARY_DIR)
    get_filename_component(QUAZIP_LIBRARY_DIR ${QUAZIP_LIBRARIES} DIRECTORY)
  endif ()

  include (DeployQt5)
  install_qt5_executable ("${vatsinator_EXECUTABLE}"
      "${QT_QPA_PLUGIN}" # qtplugins
      "" # libs
      "${QT_INSTALL_LIBS};${QUAZIP_LIBRARY_DIR}" # dirs
  )
endif ()
