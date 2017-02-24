find_package (Qt5 COMPONENTS Core Network Gui Widgets)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

if (APPLE)
    find_package (Qt5MacExtras)
    find_library (COCOA_LIBRARY Cocoa)
endif ()

include_directories (${CMAKE_CURRENT_BINARY_DIR})

set (vatsinator_SRCS
    main.cpp
    aboutwindow.cpp
    settingswindow.cpp
    vatsinatorwindow.cpp
    
    resources/icons.qrc

    $<$<BOOL:APPLE>:cocoautils.mm>
)

set (vatsinator_UIS
    settingswindow.ui
    vatsinatorwindow.ui
)

# application icon for Windows
if (WIN32)
    string (REPLACE "." "," vatsinator_VERSION_WINDRES ${vatsinator_VERSION})
    string (REGEX REPLACE "-.*" "" vatsinator_VERSION_WINDRES ${vatsinator_VERSION_WINDRES})
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/dist/vatsinator.rc.in ${CMAKE_CURRENT_BINARY_DIR}/vatsinator.rc)
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/dist/vatsinator.ico ${CMAKE_CURRENT_BINARY_DIR}/vatsinator.ico COPYONLY)
    list (APPEND vatsinator_SRCS ${CMAKE_CURRENT_BINARY_DIR}/vatsinator.rc)
endif ()

add_executable (vatsinator MACOSX_BUNDLE WIN32 ${vatsinator_SRCS} ${vatsinator_UIS})
target_link_libraries (vatsinator
    vtrwidgets
    Qt5::Widgets
    $<$<BOOL:APPLE>:Qt5::MacExtras>
    $<$<BOOL:APPLE>:${COCOA_LIBRARY}>
)

if (APPLE)
    install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/dist/vatsinator.icns DESTINATION ${vatsinator_RESOURCES_TARGET_DIR})
    set_target_properties (vatsinator PROPERTIES
        MACOSX_BUNDLE_ICON_FILE "vatsinator.icns"
        MACOSX_BUNDLE_BUNDLE_NAME "Vatsinator"
        MACOSX_BUNDLE_BUNDLE_VERSION "${vatsinator_VERSION}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.eu.vatsinator.Vatsinator"
        MACOSX_BUNDLE_INFO_STRING "${PROJECT_NAME}"
        MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_NAME} Version ${vatsinator_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${vatsinator_VERSION}"
    )
endif()

add_dependencies (vatsinator i18n)


#
# DEPLOYMENT
#
install (TARGETS vatsinator
    RUNTIME DESTINATION ${vatsinator_BINARY_TARGET_DIR}
    BUNDLE DESTINATION .
)

if (APPLE OR WIN32)
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION "${vatsinator_BINARY_TARGET_DIR}")
    include(InstallRequiredSystemLibraries)

    include (QtUtils)
    query_qmake (QT_INSTALL_BINS QT_INSTALL_BINS)
    query_qmake (QT_INSTALL_LIBS QT_INSTALL_LIBS)
    get_platform_qpa_plugin (QT_QPA_PLUGIN)

    if (WIN32)
        # find OpenSSL libraries to deploy
        find_library (LIBEAY_LOCATION NAMES libeay32)
        if (LIBEAY_LOCATION)
            list (APPEND vatsinator_3RDPARTY_LIBS ${LIBEAY_LOCATION})
        endif ()
        
        find_library (SSLEAY_LOCATION NAMES ssleay32)
        if (SSLEAY_LOCATION)
            list (APPEND vatsinator_3RDPARTY_LIBS ${SSLEAY_LOCATION})
        endif ()

        cmake_policy (PUSH)
        cmake_policy (SET CMP0026 OLD)
        get_target_property (vatsinator_EXECUTABLE vatsinator LOCATION)
        get_filename_component (vatsinator_EXECUTABLE ${vatsinator_EXECUTABLE} NAME)
        cmake_policy (POP)
    elseif (APPLE)
        set (vatsinator_EXECUTABLE vatsinator.app)
    endif ()

    cmake_policy (PUSH)
    cmake_policy (SET CMP0026 OLD)
    set (plugins # plugins to be fixed up
        euroutenotamprovider idldrawer tiledmapdrawer
        travelpayoutsairlinelogoprovider vatbookatcbookingprovider
    )
    foreach (p ${plugins})
        get_target_property (${p}_location ${p} LOCATION)
        get_filename_component (${p}_name ${${p}_location} NAME)
        list (APPEND vatsinator_LIBS ${vatsinator_PLUGINS_TARGET_DIR}/${${p}_name})
    endforeach ()
    cmake_policy (POP)

    include (DeployQt5)
    install_qt5_executable ("${vatsinator_EXECUTABLE}"
        "${QT_QPA_PLUGIN}"
        "${vatsinator_LIBS};${vatsinator_3RDPARTY_LIBS}"
        "${QT_INSTALL_LIBS};${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
    )
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

if (APPLE)
    set (install_i18n_dir "Resources/translations")
elseif (WIN32)
    set (install_i18n_dir "../translations")
else ()
    set (install_i18n_dir "translations")
endif ()

if (APPLE)
    set (install_plugins_dir "PlugIns")
else ()
    set (install_plugins_dir "plugins")
endif ()

if (APPLE OR WIN32)
    configure_file (${CMAKE_CURRENT_SOURCE_DIR}/qt.conf.in ${CMAKE_CURRENT_BINARY_DIR}/qt.conf)
    install (FILES ${CMAKE_CURRENT_BINARY_DIR}/qt.conf DESTINATION ${vatsinator_RESOURCES_TARGET_DIR})
endif ()

set (CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/dist/vatsinator.ico")
set (CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/dist/uninstall.ico")
set (CPACK_DMG_DS_STORE "${CMAKE_CURRENT_SOURCE_DIR}/dist/DS_STORE")
set (CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_CURRENT_SOURCE_DIR}/dist/dmg-background.png")

include (PackageVatsinator)
