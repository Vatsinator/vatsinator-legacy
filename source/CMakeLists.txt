set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")
include (Macros)

set (CMAKE_AUTOMOC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)

# exec file
set (vatsinator_TARGET
  vatsinator
)

if (APPLE) # bundle looks better
  set (vatsinator_TARGET
    Vatsinator
  )
endif ()

set (vatsinator_SRCS
  netconfig.cpp
  vatsinatorapplication.cpp
)

set (vatsinator_HEADERS
  netconfig.h
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
    modules
    network
    plugins
    storage
    ui
    vatsimdata
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable (${vatsinator_TARGET} WIN32
    main.cpp
    ${vatsinator_SRCS}
    ${vatsinator_HEADERS}
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
        ${vatsinator_QT_LIBS}
        Qt5::WinMain
        ${OPENGL_LIBRARY}
        ${QUAZIP_LIBRARIES}
    )
endif (LINUX)

if (${vatsinator_UI_IMPLEMENTATION} MATCHES "widgets")
  set (UI_IMPLEMENTATION "WidgetsUserInterface")
else (${vatsinator_UI_IMPLEMENTATION} MATCHES "widgets")
  message (FATAL_ERROR "Known user interface implementations: widgets")
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

get_property (vatsinator_EXECUTABLE TARGET ${vatsinator_TARGET} PROPERTY LOCATION)
get_filename_component (vatsinator_EXECUTABLE ${vatsinator_EXECUTABLE} NAME)

if (LINUX)

    install (TARGETS ${vatsinator_TARGET} RUNTIME DESTINATION bin)
    
elseif (APPLE)

    # move all files to .app bundle
    set (vatsinator_BUNDLE ${CMAKE_CURRENT_BINARY_DIR}/${vatsinator_TARGET}.app)
    
    add_custom_command (TARGET ${vatsinator_TARGET} POST_BUILD
        COMMAND mkdir ARGS -p
        ${vatsinator_BUNDLE}/Contents/Resources/
        COMMAND cp ARGS -R
        ${MACOSX_BUNDLE_ICON} ${vatsinator_BUNDLE}/Contents/Resources/
        COMMAND cp ARGS -R
        ${CMAKE_BINARY_DIR}/vatsinator.resources/* ${vatsinator_BUNDLE}/Contents/Resources/
    )
    
    install (TARGETS ${vatsinator_TARGET} DESTINATION .)

    exec_program (${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_BINS"
        OUTPUT_VARIABLE QT_INSTALL_LIBS)
    exec_program (${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_PLUGINS"
        OUTPUT_VARIABLE QT_INSTALL_PLUGINS)
    # setup path to libqcocoa.dylib
    set (QT_QCOCOA_PLUGIN ${QT_INSTALL_PLUGINS}/platforms/libqcocoa.dylib)
    
    include (DeployQt5)
    install_qt5_executable ("${vatsinator_TARGET}.app"
        "${QT_QCOCOA_PLUGIN}" # qtplugins
        "" # libs
        "/usr/local/lib" # dirs
    )
    
elseif (WIN32)

    install (TARGETS ${vatsinator_TARGET} DESTINATION .)
    
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
    include(InstallRequiredSystemLibraries)
    
    exec_program (${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_BINS"
        OUTPUT_VARIABLE QT_INSTALL_LIBS)
    exec_program (${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_PLUGINS"
        OUTPUT_VARIABLE QT_INSTALL_PLUGINS)
    
    # setup path to qwindows.dll
    if (MSVC)
        set (QT_QWINDOWS_PLUGIN ${QT_INSTALL_PLUGINS}/platforms/qwindows.dll)
    endif ()
    
    include (DeployQt5)
    install_qt5_executable ("${vatsinator_EXECUTABLE}"
        "${QT_QWINDOWS_PLUGIN}" # qtplugins
        "" # libs
        "${QT_INSTALL_LIBS};${QUAZIP_LIBRARY_DIR}" # dirs
    )
    
endif (LINUX)
