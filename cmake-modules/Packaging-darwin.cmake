set (CPACK_GENERATOR "DragNDrop")

include (CPackDefaultOptions)

if (APPLE AND CMAKE_INSTALL_PREFIX MATCHES "/usr/local")
    set (CMAKE_INSTALL_PREFIX "/Applications")
endif (APPLE AND CMAKE_INSTALL_PREFIX MATCHES "/usr/local")

string (REPLACE "_" "-" CPACK_PACKAGE_VERSION ${vatsinator_VERSION})
# 
set (CPACK_DMG_FORMAT "UDBZ")
set (CPACK_DMG_VOLUME_NAME "Vatsinator")
set (CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}")
set (CPACK_BUNDLE_STARTUP_COMMAND "${CMAKE_CURRENT_BINARY_DIR}/vatsinator")

set (CPACK_PACKAGE_EXECUTABLES "vatsinator")

set (MACOSX_BUNDLE_ICON_FILE vatsinator.icns)
set (CPACK_PACKAGE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/source/ui/resources/vatsinator.icns)
set (CPACK_BUNDLE_NAME "Vatsinator")
set (CPACK_BUNDLE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/source/ui/resources/vatsinator.icns)
set (CPACK_BUNDLE_PLIST "Info.plist")

set_source_files_properties (
    ${CMAKE_CURRENT_SOURCE_DIR}/source/ui/resources/vatsinator.icns
    PROPERTIES MACOSX_PACKAGE_LOCATION Resources
)

include (CPack)
