#string (REPLACE "_" "." CPACK_PACKAGE_VERSION ${vatsinator_VERSION})

set (CPACK_GENERATOR "NSIS")

include (CPackDefaultOptions)

set (CPACK_PACKAGE_FILE_NAME "VatsinatorSetup-${vatsinator_VERSION}")
set (CPACK_PACKAGE_EXECUTABLES "vatsinator;Vatsinator")
set (CPACK_PACKAGE_INSTALL_DIRECTORY "Vatsinator")
set (CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}\\\\dist\\\\install.bmp")

set (CPACK_NSIS_EXECUTABLES_DIRECTORY ".")

set (CPACK_NSIS_PACKAGE_NAME "Vatsinator")
set (CPACK_NSIS_DISPLAY_NAME "Vatsinator")

set (CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/dist/icon.ico")
set (CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}/dist/uninstall.ico")
set (CPACK_NSIS_DISPLAY_NAME "Vatsinator")
set (CPACK_NSIS_HELP_LINK "http://vatsinator.eu.org/")
set (CPACK_NSIS_URL_INFO_ABOUT "http://vatsinator.eu.org/")
set (CPACK_NSIS_CONTACT "michal@garapich.pl")
set (CPACK_NSIS_MUI_FINISHPAGE_RUN "vatsinator.exe")
set (CPACK_NSIS_INSTALLED_ICON_NAME "vatsinator.exe")
set (CPACK_NSIS_MENU_LINKS "http://vatsinator.eu.org/" "Vatsinator Homepage")

set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    CreateShortCut \\\"$DESKTOP\\\\Vatsinator.lnk\\\" \\\"$INSTDIR\\\\vatsinator.exe\\\"
")


set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
    Delete \\\"$DESKTOP\\\\Vatsinator.lnk\\\"
")

include (CPack)
