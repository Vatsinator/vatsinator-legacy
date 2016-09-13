find_package (Qt5Widgets REQUIRED)
include_directories (${Qt5Widgets_INCLUDE_DIRS})
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

include_directories (${CMAKE_CURRENT_BINARY_DIR})

set (vatsinator_SRCS
    main.cpp
    aboutwindow.cpp
    settingswindow.cpp
    vatsinatorwindow.cpp
    
    resources/icons.qrc
)

# application icon for Windows
if (WIN32)
    string (REPLACE "." "," vatsinator_VERSION_WINDRES ${vatsinator_VERSION})
    string (REGEX REPLACE "-.*" "" vatsinator_VERSION_WINDRES ${vatsinator_VERSION_WINDRES})
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/dist/vatsinator.rc.in ${CMAKE_CURRENT_BINARY_DIR}/vatsinator.rc)
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/dist/vatsinator.ico ${CMAKE_CURRENT_BINARY_DIR}/vatsinator.ico COPYONLY)
    list (APPEND vatsinator_SRCS ${CMAKE_CURRENT_BINARY_DIR}/vatsinator.rc)
endif ()

add_executable (vatsinator MACOSX_BUNDLE WIN32 ${vatsinator_SRCS})
target_link_libraries (vatsinator
    Qt5::Widgets
    vtrwidgets
)
add_dependencies (vatsinator i18n)
install (TARGETS vatsinator DESTINATION bin)

if (APPLE OR WIN32)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "bin")
    include(InstallRequiredSystemLibraries)

    include (QtUtils)
    query_qmake (QT_INSTALL_LIBS QT_INSTALL_BINS)
    get_platform_qpa_plugin (QT_QPA_PLUGIN)
    
    list (APPEND vatsinator_3RDPARTY_LIBS ${QT_QPA_PLUGIN})

    if (WIN32)
        # find OpenSSL libraries to deploy
        find_library (LIBEAY_LOCATION NAMES libeay32.dll)
        if (LIBEAY_LOCATION)
            list (APPEND vatsinator_3RDPARTY_LIBS ${LIBEAY_LOCATION})
        endif ()
        
        find_library (SSLEAY_LOCATION NAMES ssleay32.dll)
        if (SSLEAY_LOCATION)
            list (APPEND vatsinator_3RDPARTY_LIBS ${SSLEAY_LOCATION})
        endif ()

        cmake_policy (PUSH)
        cmake_policy (SET CMP0026 OLD)
        get_target_property (vatsinator_EXECUTABLE vatsinator LOCATION)
        get_filename_component (vatsinator_EXECUTABLE ${vatsinator_EXECUTABLE} NAME)
        cmake_policy (POP)
    elseif (APPLE)
        set (vatsinator_EXECUTABLE Vatsinator.app)
    endif ()

    include (DeployQt5)
    install_qt5_executable ("bin/${vatsinator_EXECUTABLE}" "${vatsinator_3RDPARTY_LIBS}" "" "${QT_INSTALL_LIBS}")
endif ()

if (LINUX)
    configure_file (
        ${CMAKE_CURRENT_SOURCE_DIR}/dist/vatsinator.desktop.in
        ${CMAKE_CURRENT_BINARY_DIR}/dist/vatsinator.desktop
    )

    install (FILES ${CMAKE_CURRENT_BINARY_DIR}/dist/vatsinator.desktop
        DESTINATION share/applications)

    file (GLOB icons
        RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/dist
        "${CMAKE_CURRENT_SOURCE_DIR}/dist/vatsinator-*.png")

    foreach (icon ${icons})
        string (REGEX
            REPLACE "^vatsinator-([0-9]+).png$"
            "\\1"
            size ${icon}
        )

        install (FILES dist/${icon} DESTINATION share/icons/hicolor/${size}x${size}/apps RENAME vatsinator.png)
    endforeach ()
endif ()

include (PackageVatsinator)
