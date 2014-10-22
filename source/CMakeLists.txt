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

set (vatsinator_SUBDIRS
    db
    events
    modules
    network
    plugins
    storage
    ui
    vatsimdata
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable (${vatsinator_TARGET} MACOSX_BUNDLE
    main.cpp
    ${vatsinator_SRCS}
)

set (vatsinator_QT_LIBS
    Qt5::Widgets
    Qt5::Network
    Qt5::Xml
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
        mingw32
        ${OPENGL_gl_LIBRARY}
        ${QT_QTCORE_LIBRARY}
        ${QT_QTGUI_LIBRARY}
        ${QT_QTOPENGL_LIBRARY}
        ${QT_QTNETWORK_LIBRARY}
        ${QUAZIP_LIBRARIES}
  )
endif (LINUX)

if (${vatsinator_UI_IMPLEMENTATION} MATCHES "widgets")
  set (UI_IMPLEMENTATION "WidgetsUserInterface")
else (${vatsinator_UI_IMPLEMENTATION} MATCHES "widgets")
  message(FATAL_ERROR "Known user interface implementations: widgets")
endif (${vatsinator_UI_IMPLEMENTATION} MATCHES "widgets")

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
    target_link_libraries (${vatsinator_TARGET}
        -Wl,--start-group
        -Wl,--whole-archive
        ${vatsinator_SUBDIRS}
        -Wl,--no-whole-archive
        -Wl,--end-group
    )
endif (APPLE)

target_link_libraries (${vatsinator_TARGET} ${vatsinator_LIBS})


if (LINUX)

    install (TARGETS ${vatsinator_TARGET} RUNTIME DESTINATION bin)
    
elseif (APPLE)

    # move all files to .app bundle
    set (vatsinator_BUNDLE ${vatsinator_TARGET}.app)
    install (TARGETS ${vatsinator_TARGET} DESTINATION .)

    exec_program (${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_BINS"
        OUTPUT_VARIABLE QT_INSTALL_LIBS)
    exec_program (${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_PLUGINS"
        OUTPUT_VARIABLE QT_INSTALL_PLUGINS)
    # setup path to libqcocoa.dylib
    set (QT_QCOCOA_PLUGIN ${QT_INSTALL_PLUGINS}/platforms/libqcocoa.dylib)    

    include (DeployQt5)
    install_qt5_executable ("${vatsinator_BUNDLE}"
        "${QT_QCOCOA_PLUGIN}" # qtplugins
        "" # libs
        "/usr/local/lib" # dirs
    )
    
elseif (WIN32)

    install (TARGETS ${vatsinator_TARGET} DESTINATION .)
    
endif (LINUX)
