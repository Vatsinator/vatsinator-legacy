set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

# exec file
set (vatsinator_TARGET
    vatsinator
)

if (APPLE) # bundle looks better
  set (vatsinator_TARGET
      Vatsinator
  )
endif (APPLE)

set (vatsinator_SRCS
    netconfig.cpp
    vatsinatorapplication.cpp
)

set (vatsinator_MOC_SRCS
    vatsinatorapplication.h
)

set (vatsinator_SUBDIRS
    db
    events
    glutils
    modules
    network
    plugins
    storage
    ui
    vatsimdata
)

qt4_wrap_cpp (vatsinator_MOC_FILES ${vatsinator_MOC_SRCS})

include (Macros)
if (CMAKE_COMPILER_IS_GNUCC)
    include (GccVersion)
        
    # workaround for gcc <= 4.6
    if (NOT GCC_VERSION_47)
      add_definitions("-Doverride=" "")
    endif (NOT GCC_VERSION_47)
endif (CMAKE_COMPILER_IS_GNUCC)

set (QT_USE_QTOPENGL TRUE)
set (QT_USE_QTNETWORK TRUE)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable (${vatsinator_TARGET} MACOSX_BUNDLE WIN32
    main.cpp
    ${vatsinator_SRCS}
    ${vatsinator_MOC_FILES}
)

# fix for older versions of qjson library
if (qjson_LIBRARIES)
    set (QJSON_LIBRARIES ${qjson_LIBRARIES})
endif (qjson_LIBRARIES)

# Set PREFIX for defines.h and necessary libs
if (LINUX)
    set (vatsinator_PREFIX "${CMAKE_INSTALL_PREFIX}/share/vatsinator/")

    set (vatsinator_LIBS
        ${QT_LIBRARIES}
        ${OPENGL_LIBRARY}
        ${QUAZIP_LIBRARIES}
        ${QJSON_LIBRARIES}
    )
elseif (APPLE)
    set (vatsinator_PREFIX "")

    set (vatsinator_LIBS
        ${QT_LIBRARIES}
        ${OPENGL_LIBRARY}
        ${QUAZIP_LIBRARIES}
        ${QJSON_LIBRARIES}
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
        ${QJSON_LIBRARIES}
  )
  
  # disable VBO on Win32 build - MinGW doesn't handle it properly
  set (CONFIG_NO_VBO 1)
endif (LINUX)

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
    
    include (DeployQt4)
    install_qt4_executable ("${vatsinator_TARGET}.app"
        "" # qtplugins
        "" # libs
        "/usr/local/lib" # dirs
    )
    
elseif (WIN32)

    install (TARGETS ${vatsinator_TARGET} DESTINATION .)
    
endif (LINUX)
