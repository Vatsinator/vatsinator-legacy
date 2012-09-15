if (NOT vatsinator_DESCRIPTION)
	file (READ ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules/DESCRIPTION vatsinator_DESCRIPTION)
endif (NOT vatsinator_DESCRIPTION)

string (REPLACE "_" "." CPACK_PACKAGE_VERSION ${vatsinator_VERSION})

set (CPACK_GENERATOR "DEB;RPM;TBZ2")
set (CPACK_PACKAGE_CONTACT "Michał Garapich michal@garapich.pl")
set (CPACK_PACKAGE_NAME "Vatsinator")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")

set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Vatsim monitor")
set (CPACK_PACKAGE_FILE_NAME "vatsinator-${vatsinator_VERSION}-Linux-x86_64")
set (CPACK_SOURCE_PACKAGE_FILE_NAME "vatsinator-${vatsinator_VERSION}")
set (CPACK_PACKAGE_DESCRIPTION "${vatsinator_DESCRIPTION}")

set (CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set (CPACK_DEBIAN_PACKAGE_DEPENDS "libqt4-core (>=4.7), libqt4-gui (>=4.7), libqt4-opengl (>=4.7), libqt4-network (>=4.7)")
set (CPACK_DEBIAN_BUILD_DEPENDS "debhelper, cmake, libqt4-opengl-dev (>=4.7), qt4-qmake (>=4.7)")
set (CPACK_DEBIAN_PACKAGE_SECTION "web")
set (CPACK_DEBIAN_CHANGELOG "  * Initial release")
set (CPACK_DEBIAN_PACKAGE_PRIORITY extra)
set (CPACK_DEBIAN_PACKAGE_SECTION web)
set (CPACK_DEBIAN_CMAKE_OPTIONS "")
set (CPACK_DEBIAN_PACKAGE_SUGGESTS "metar")

set (DPUT_HOST ppa:michal-k93/vatsinator)

set (CPACK_DEBIAN_DISTRIBUTION_RELEASES precise)
set (CPACK_DEBIAN_DISTRIBUTION_NAME ubuntu)

set (CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set (CPACK_RPM_PACKAGE_DESCRIPTION "The simple Vatsim monitor that shows logged-in pilots, ATCs and provides some information useful during flights.")
set (CPACK_RPM_PACKAGE_LICENSE "GPL-3")
set (CPACK_RPM_PACKAGE_GROUP "Applications/Internet")


include (CPack)
include (DebSourcePPA)

