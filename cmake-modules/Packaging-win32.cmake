#string (REPLACE "_" "." CPACK_PACKAGE_VERSION ${vatsinator_VERSION})
set (CPACK_PACKAGE_VERSION "")

set (CPACK_GENERATOR "NSIS")
set (CPACK_PACKAGE_CONTACT "Michał Garapich michal@garapich.pl")
set (CPACK_PACKAGE_NAME "Vatsinator")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")

set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Vatsim monitor")
set (CPACK_PACKAGE_FILE_NAME "vatsinator-${vatsinator_VERSION}")

set (CPACK_NSIS_MUI_ICON "${CMAKE_CURRENT_SOURCE_DIR}/source/ui/resources/icon.ico")
set (CPACK_NSIS_MUI_UNIICON "${CMAKE_CURRENT_SOURCE_DIR}/source/ui/resources/icon.ico")
set (CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/resources/vatsinator-24.png")
set (CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} Vatsinator")
set (CPACK_NSIS_HELP_LINK "https://github.com/Garrappachc/Vatsinator")
set (CPACK_NSIS_URL_INFO_ABOUT "https://github.com/Garrappachc/Vatsinator")
set (CPACK_NSIS_CONTACT "michal@garapich.pl")
set (CPACK_NSIS_MUI_FINISHPAGE_RUN "vatsinator.exe")

include (CPack)