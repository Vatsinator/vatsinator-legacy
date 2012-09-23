set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

set (vatsinator_TARGET
    vatsinator
)

if (MINGW)
    set(vatsinator_TARGET
        ${vatsinator_TARGET}.exe)
endif (MINGW)

set (vatsinator_SRCS
    vatsinatorapplication.cpp
)

if (APPLE)
    set (vatsinator_SRCS
        ${vatsinator_SRCS}
        ${CMAKE_CURRENT_SOURCE_DIR}/ui/resources/vatsinator.icns
    )
endif (APPLE)


set (vatsinator_MOC_SRCS
    vatsinatorapplication.h
)

set (vatsinator_SUBDIRS
    cache
    db
    glutils
    modules
    network
    settings
    ui
    vatsimdata
)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    set (vatsinator_SUBDIRS
        vdebug
        ${vatsinator_SUBDIRS}
    )
endif (CMAKE_BUILD_TYPE MATCHES Debug)

qt4_wrap_cpp (vatsinator_MOC_FILES ${vatsinator_MOC_SRCS})

include (Macros)
include (Translations)

set(QT_USE_QTOPENGL TRUE)
set(QT_USE_QTNETWORK TRUE)

include(${QT_USE_FILE})

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_definitions(${QT_DEFINITIONS})

add_executable (${vatsinator_TARGET} WIN32
    main.cpp
    ${vatsinator_SRCS}
    ${vatsinator_MOC_FILES}
    ${vatsinator_TRANSLATIONS}
)

if (UNIX AND NOT APPLE AND NOT MINGW)
    # set -DPREFIX for defines.h
    set (vatsinator_PREFIX "${CMAKE_INSTALL_PREFIX}/share/vatsinator/")

    set (vatsinator_LIBS
        ${QT_LIBRARIES}
        ${OPENGL_LIBRARY}
    )
elseif (APPLE)
    set (vatsinator_PREFIX "")

    set (vatsinator_LIBS
        ${QT_LIBRARIES}
        ${OPENGL_LIBRARY}
    )
else (UNIX AND NOT APPLE AND NOT MINGW)
    set (vatsinator_PREFIX "")

    set (vatsinator_LIBS
        mingw32
        ${OPENGL_gl_LIBRARY}
        ${QT_QTCORE_LIBRARY}
        ${QT_QTGUI_LIBRARY}
        ${QT_QTOPENGL_LIBRARY}
        ${QT_QTNETWORK_LIBRARY}
  )
endif (UNIX AND NOT APPLE AND NOT MINGW)

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

if (UNIX AND NOT APPLE AND NOT MINGW)
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
    install (TARGETS ${vatsinator_TARGET}
        BUNDLE DESTINATION . COMPONENT Runtime
    )
    
    install (FILES
        ${LANGUAGE_FILES}
        ${vatsinator_TRANSLATIONS}
        DESTINATION ${CMAKE_PROJECT_NAME}.app/Contents/Resources/translations
    )
elseif (MINGW)
    install (PROGRAMS
        ${CMAKE_CURRENT_BINARY_DIR}/vatsinator.exe
        DESTINATION .
    )
    
    install (FILES
        ${LANGUAGE_FILES}
        ${vatsinator_TRANSLATIONS}
        DESTINATION ./translations
    )
endif (UNIX AND NOT APPLE AND NOT MINGW)
