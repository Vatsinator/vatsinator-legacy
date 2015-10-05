set (models_SRCS
    airporttablemodel.cpp
    atctablemodel.cpp
    flighttablemodel.cpp
    metarlistmodel.cpp
)

set (models_HEADERS
    airporttablemodel.h
    atctablemodel.h
    flighttablemodel.h
    metarlistmodel.h
    roles.h
)

add_library (models OBJECT
    ${models_SRCS}
    ${models_HEADERS}
)


