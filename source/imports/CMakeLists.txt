if (NOT ANDROID)
    message (FATAL_ERROR "Imports are valid only on Android")
endif ()

add_definitions (-DQT_PLUGIN)
add_definitions (-DQT_SHARED)

add_subdirectory (map)
add_subdirectory (vatsim)