set (vatsinator_RESOURCES
    ${CMAKE_SOURCE_DIR}/COPYING
    Manifest
    WorldAirports.db
    WorldFirs.db
    WorldMap.db)

vatsinator_install (
    "${vatsinator_RESOURCES}" "")

set (vatsinator_RESOURCES_DATA
    data/airlines
    data/alias
    data/country
    data/fir
    data/model
    data/uir)

vatsinator_install (
    "${vatsinator_RESOURCES_DATA}" "/data")