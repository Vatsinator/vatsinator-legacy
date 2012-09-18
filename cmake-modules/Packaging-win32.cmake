#string (REPLACE "_" "." CPACK_PACKAGE_VERSION ${vatsinator_VERSION})

set (CPACK_GENERATOR "NSIS")

include (CPackDefaultOptions)

set (CPACK_NSIS_EXECUTABLES_DIRECTORY ".")

set (CPACK_NSIS_PACKAGE_NAME "Vatsinator")
set (CPACK_NSIS_DISPLAY_NAME "Vatsinator")

set (CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/source/ui/resources/icon.ico")
set (CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/source/ui/resources/uninstall.ico")
set (CPACK_NSIS_DISPLAY_NAME "Vatsinator")
set (CPACK_NSIS_HELP_LINK "https://github.com/Garrappachc/Vatsinator")
set (CPACK_NSIS_URL_INFO_ABOUT "https://github.com/Garrappachc/Vatsinator")
set (CPACK_NSIS_CONTACT "michal@garapich.pl")
set (CPACK_NSIS_MUI_FINISHPAGE_RUN "vatsinator.exe")
set (CPACK_NSIS_INSTALLED_ICON_NAME "vatsinator.exe")
set (CPACK_NSIS_MENU_LINKS "https://github.com/Garrappachc/Vatsinator" "Vatsinator Homepage")

set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
	CreateShortCut \\\"$DESKTOP\\\\Vatsinator.lnk\\\" \\\"$INSTDIR\\\\vatsinator.exe\\\"
")


set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
	Delete \\\"$DESKTOP\\\\Vatsinator.lnk\\\"
")

include (CPack)
