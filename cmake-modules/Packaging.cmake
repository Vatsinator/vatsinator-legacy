set (CPACK_PACKAGE_CONTACT "Michał Garapich michal@garapich.pl")
set (CPACK_PACKAGE_NAME "Vatsinator")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")

set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Vatsim monitor")

# Platform-specific package setup
if (LINUX)  
  # Creates deb/rpm
  include (PackagingLinux)
elseif (APPLE)
  # Creates dmg
  include (PackagingOSX)
elseif (WIN32)
  # NSIS installer
  include (PackagingWin32)  
endif ()

include (CPack)
