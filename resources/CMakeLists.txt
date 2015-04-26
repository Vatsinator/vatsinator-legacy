install (
    FILES
        ${CMAKE_SOURCE_DIR}/COPYING
        Manifest
        WorldAirports.db
        WorldFirs.db
    DESTINATION
        ${vatsinator_INSTALL_DIR}
)

install(
    FILES
        data/airlines
        data/alias
        data/country
        data/fir
        data/model
        data/tma
        data/uir
    DESTINATION
        ${vatsinator_INSTALL_DIR}/data
)
