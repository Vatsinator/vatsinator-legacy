if (CMAKE_INSTALL_PREFIX MATCHES "/usr/local")
  set (CMAKE_INSTALL_PREFIX "/Applications")
endif ()

set (vatsinator_BUNDLE source/${CMAKE_PROJECT_NAME}.app)

set (MACOSX_BUNDLE_INFO_STRING "Vatsinator ${vatsinator_VERSION}")
set (MACOSX_BUNDLE_BUNDLE_VERSION "Vatsinator ${vatsinator_VERSION}")
set (MACOSX_BUNDLE_ICON_FILE "vatsinator.icns")
set (MACOSX_BUNDLE_VERSION "${vatsinator_VERSION}")
set (MACOSX_BUNDLE_SHORT_VERSION_STRING "${vatsinator_VERSION}")
set (MACOSX_BUNDLE_LONG_VERSION_STRING "Version ${vatsinator_VERSION}")
set (MACOSX_BUNDLE_COPYRIGHT "2015 by VatsinatorTeam")
set (MACOSX_BUNDLE_GUI_IDENTIFIER "org.eu.vatsinator")
set (MACOSX_BUNDLE_BUNDLE_NAME "Vatsinator")

set (MACOSX_BUNDLE_RESOURCES "${CMAKE_CURRENT_BINARY_DIR}/${vatsinator_BUNDLE}/Contents/Resources")
set (MACOSX_BUNDLE_ICON "${PROJECT_SOURCE_DIR}/dist/${MACOSX_BUNDLE_ICON_FILE}")

set (CPACK_GENERATOR "DragNDrop")
set (CPACK_DMG_FORMAT "UDBZ")
set (CPACK_DMG_VOLUME_NAME "Vatsinator")
set (CPACK_SYSTEM_NAME "OSX")
set (CPACK_PACKAGE_FILE_NAME "Vatsinator-${vatsinator_VERSION}")

set (CPACK_DMG_DS_STORE "${PROJECT_SOURCE_DIR}/dist/DS_Store")
set (CPACK_DMG_BACKGROUND_IMAGE "${PROJECT_SOURCE_DIR}/dist/dmg-background.png")
