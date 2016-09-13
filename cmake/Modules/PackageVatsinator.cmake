set (CPACK_PACKAGE_NAME "Vatsinator")
set (CPACK_PACKAGE_VENDOR "Vatsinator Team")
set (CPACK_PACKAGE_CONTACT "Michał Garapich michal@garapich.pl")
set (CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/COPYING")
set (CPACK_PACKAGE_VERSION ${vatsinator_VERSION})
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Vatsim monitor")
set (CPACK_PACKAGE_EXECUTABLES "vatsinator;Vatsinator")

if (WIN32)
    set (CPACK_GENERATOR "NSIS")
    
    set (CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}Setup-${vatsinator_VERSION}")
    set (CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}")
    set (CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\dist\\\\install.bmp")
    
    set (CPACK_NSIS_PACKAGE_NAME "${CPACK_PACKAGE_NAME}")
    set (CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_NAME}")
    set (CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/dist/vatsinator.ico")
    set (CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/dist/uninstall.ico")
    set (CPACK_NSIS_HELP_LINK "http://vatsinator.eu.org/")
    set (CPACK_NSIS_URL_INFO_ABOUT "http://vatsinator.eu.org/")
    set (CPACK_NSIS_CONTACT "michal@garapich.pl")
    set (CPACK_NSIS_MUI_FINISHPAGE_RUN "vatsinator.exe")
    set (CPACK_NSIS_MENU_LINKS "http://vatsinator.eu.org/" "Vatsinator Homepage")
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
        CreateShortCut \\\"$DESKTOP\\\\Vatsinator.lnk\\\" \\\"$INSTDIR\\\\bin\\\\vatsinator.exe\\\"
    ")
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
        Delete \\\"$DESKTOP\\\\Vatsinator.lnk\\\"
    ")
elseif (APPLE)
    set (CPACK_GENERATOR "DragNDrop")

    set (CPACK_DMG_FORMAT "UBDZ")
    set (CPACK_DMG_VOLUME_NAME "Vatsinator")
    set (CPACK_PACKAGE_FILE_NAME "Vatsinator-${vatsinator_VERSION}")

    set (CPACK_DMG_DS_STORE "${CMAKE_CURRENT_SOURCE_DIR}/dist/DS_STORE")
    set (CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_CURRENT_SOURCE_DIR}/dist/dmg-background.png")
endif ()

include (CPack)
