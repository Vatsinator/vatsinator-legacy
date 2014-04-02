set (vatsinator_RESOURCES
    ${CMAKE_SOURCE_DIR}/COPYING
    Manifest
    WorldAirports.db
    WorldFirs.db
    WorldMap.db)

vatsinator_install (
    "${vatsinator_RESOURCES}" "")

set (vatsinator_RESOURCES_PIXMAPS
    pixmaps/1p32.png
    pixmaps/2p32.png
    pixmaps/4p32.png
    pixmaps/2j32.png
    pixmaps/3j32.png
    pixmaps/4j32.png
    pixmaps/conc32.png)

vatsinator_install (
    "${vatsinator_RESOURCES_PIXMAPS}" "/pixmaps")

set (vatsinator_RESOURCES_DATA
    data/airlines
    data/alias
    data/country
    data/fir
    data/model
    data/uir)

vatsinator_install (
    "${vatsinator_RESOURCES_DATA}" "/data")