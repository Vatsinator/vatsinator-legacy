set (CPACK_GENERATOR "DEB;RPM")
set (CPACK_PACKAGE_CONTACT "Michał Garapich garrappachc@gmail.com")
set (CPACK_PACKAGE_NAME "Vatsinator")

string (REPLACE "_" "." CPACK_PACKAGE_VERSION ${vatsinator_VERSION})
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Vatsim monitor")
set (CPACK_PACKAGE_FILE_NAME "vatsinator-${vatsinator_VERSION}-Linux-x86_64")

set (CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set (CPACK_DEBIAN_PACKAGE_DEPENDS "libqt4-core (>=4.7), libqt4-gui (>=4.7), libqt4-opengl (>=4.7), libqt4-network (>=4.7)")
set (CPACK_DEBIAN_PACKAGE_SECTION "web")

set (CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set (CPACK_RPM_PACKAGE_DESCRIPTION "The simple Vatsim monitor that shows logged-in pilots, ATCs and provides some information useful during flights.")
set (CPACK_RPM_PACKAGE_LICENSE "GPL-3")
set (CPACK_RPM_PACKAGE_GROUP "Applications/Internet")


include (CPack)

