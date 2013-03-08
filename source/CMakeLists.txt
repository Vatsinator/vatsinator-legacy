set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

# exec file
set (vatsinator_TARGET
    vatsinator
)


set (vatsinator_SRCS
    vatsinatorapplication.cpp
)

set (vatsinator_MOC_SRCS
    vatsinatorapplication.h
)

set (vatsinator_SUBDIRS
    db
    glutils
    modules
    network
    storage
    ui
    vatsimdata
)

# We can't name it "debug" directory as it conflicts with cmake syntax
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set (vatsinator_SUBDIRS
        debugging
        ${vatsinator_SUBDIRS}
    )
endif (CMAKE_BUILD_TYPE MATCHES Debug)

qt4_wrap_cpp (vatsinator_MOC_FILES ${vatsinator_MOC_SRCS})

include (Macros)
include (Translations)

set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTNETWORK TRUE)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable (${vatsinator_TARGET} MACOSX_BUNDLE WIN32
    main.cpp
    ${vatsinator_SRCS}
    ${vatsinator_MOC_FILES}
    ${vatsinator_TRANSLATIONS}
)

# Set PREFIX for defines.h and necessary libs
if (LINUX)
    set (vatsinator_PREFIX "${CMAKE_INSTALL_PREFIX}/share/vatsinator/")

    set (vatsinator_LIBS
        ${QT_LIBRARIES}
        ${OPENGL_LIBRARY}
        ${QUAZIP_LIBRARIES}
    )
elseif (APPLE)
    set (vatsinator_PREFIX "")

    set (vatsinator_LIBS
        ${QT_LIBRARIES}
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
  
  # disable VBO on Win32 build - MinGW doesn't handle it properly
  set (CONFIG_NO_VBO 1)
endif (LINUX)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/vatsinator-config.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

foreach (DIR ${vatsinator_SUBDIRS})
    add_subdirectory (${DIR})
    add_dependencies (${vatsinator_TARGET}
        ${DIR}
    )
endforeach (DIR)


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
    install (PROGRAMS
        ${CMAKE_CURRENT_BINARY_DIR}/vatsinator
        DESTINATION bin
    )
    
    install (FILES
        ${vatsinator_TRANSLATIONS}
        ${LANGUAGE_FILES}
        DESTINATION share/vatsinator/translations
    )
elseif (APPLE)
    # move all files to .app bundle
    add_custom_command (TARGET ${vatsinator_TARGET} POST_BUILD
        COMMAND mkdir ARGS -p
        ${CMAKE_CURRENT_BINARY_DIR}/${vatsinator_TARGET}.app/Contents/Resources
        COMMAND cp ARGS ${MACOSX_BUNDLE_ICON} ${vatsinator_BUNDLE_RESOURCES}
        ${CMAKE_CURRENT_BINARY_DIR}/${vatsinator_TARGET}.app/Contents/Resources
        COMMAND mkdir ARGS -p
        ${CMAKE_CURRENT_BINARY_DIR}/${vatsinator_TARGET}.app/Contents/Resources/pixmaps
        COMMAND cp ARGS ${vatsinator_BUNDLE_RESOURCES_pixmaps}
        ${CMAKE_CURRENT_BINARY_DIR}/${vatsinator_TARGET}.app/Contents/Resources/pixmaps
        COMMAND mkdir ARGS -p
        ${CMAKE_CURRENT_BINARY_DIR}/${vatsinator_TARGET}.app/Contents/Resources/data
        COMMAND cp ARGS ${vatsinator_BUNDLE_RESOURCES_data}
        ${CMAKE_CURRENT_BINARY_DIR}/${vatsinator_TARGET}.app/Contents/Resources/data
        COMMAND mkdir ARGS -p
        ${CMAKE_CURRENT_BINARY_DIR}/${vatsinator_TARGET}.app/Contents/Resources/translations
        COMMAND cp ARGS ${vatsinator_TRANSLATIONS} ${LANGUAGE_FILES}
        ${CMAKE_CURRENT_BINARY_DIR}/${vatsinator_TARGET}.app/Contents/Resources/translations
    )
    
    install (TARGETS ${vatsinator_TARGET} DESTINATION .)
    
    include (DeployQt4)
    install_qt4_executable ("${vatsinator_TARGET}.app")
elseif (WIN32)
    install (PROGRAMS
        ${CMAKE_CURRENT_BINARY_DIR}/vatsinator.exe
        DESTINATION .
    )
    
    install (FILES
        ${LANGUAGE_FILES}
        ${vatsinator_TRANSLATIONS}
        DESTINATION ./translations
    )
endif (LINUX)
