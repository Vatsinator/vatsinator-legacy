if (CMAKE_INSTALL_PREFIX MATCHES "/usr/local")
    set (CMAKE_INSTALL_PREFIX "/Applications")
endif ()

set (CPACK_GENERATOR "DragNDrop")
set (CPACK_DMG_FORMAT "UDBZ")
set (CPACK_DMG_VOLUME_NAME "Vatsinator")
set (CPACK_PACKAGE_FILE_NAME "Vatsinator-${vatsinator_VERSION}")

set (CPACK_DMG_DS_STORE "${PROJECT_SOURCE_DIR}/dist/DS_Store")
set (CPACK_DMG_BACKGROUND_IMAGE "${PROJECT_SOURCE_DIR}/dist/dmg-background.png")
