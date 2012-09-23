set (CPACK_GENERATOR "DragNDrop")

include (CPackDefaultOptions)

if (APPLE AND CMAKE_INSTALL_PREFIX MATCHES "/usr/local")
    set (CMAKE_INSTALL_PREFIX "/Applications")
endif (APPLE AND CMAKE_INSTALL_PREFIX MATCHES "/usr/local")

string (REPLACE "_" "-" CPACK_PACKAGE_VERSION ${vatsinator_VERSION})

set (CPACK_COMPONENT_LIBRARIES_HIDDEN TRUE)

set (MACOSX_BUNDLE_BUNDLE_NAME Vatsinator)
set (MACOSX_BUNDLE_BUNDLE_VERSION ${vatsinator_VERSION})
set (MACOSX_BUNDLE_SHORT_VERSION_STRING ${vatsinator_VERSION})
set (MACOSX_BUNDLE_LONG_VERSION_STRING "Version ${vatsinator_VERSION}")

set (CPACK_PACKAGE_EXECUTABLES "vatsinator")
set (CPACK_PACKAGE_FILE_NAME "vatsinator")
set (CPACK_BUNDLE_ICON ${CMAKE_CURRENT_SOURCE_DIR}/source/ui/resources/vatsinator.icns)
set (CPACK_BUNDLE_PLIST "Info.plist")

include (CPack)
