if (NOT ANDROID)
    message (FATAL_ERROR "Imports are valid only on Android")
endif ()

# where QML plugins will be deployed. This value must
# be in the QML import path list.
set (vatsinator_INSTALL_QML_PLUGINS qml)

# Sets all necessary properites for the given target, making it valid
# as a QML plugin.
function (vatsinator_add_qml_plugin target plugin_dir)
    set (qmldir_file ${CMAKE_CURRENT_SOURCE_DIR}/qmldir)
    if (NOT EXISTS ${qmldir_file})
        message (FATAL_ERROR "Error adding QML plugin: could not find ${qmldir_file}")
    endif ()
    
    set_target_properties (${target} PROPERTIES
        PLUGIN_PREFIX ${vatsinator_INSTALL_QML_PLUGINS}
        PLUGIN_DIR ${plugin_dir}
        QMLDIR_FILE ${qmldir_file}
    )
    
    include(CMakeParseArguments)
    cmake_parse_arguments (arg "" "" "QMLS" ${ARGN})
    if (arg_QMLS)
        set_target_properties (${target} PROPERTIES
            QMLS "${arg_QMLS}"
        )
    endif ()
endfunction ()

add_definitions (-DQT_PLUGIN)
add_definitions (-DQT_SHARED)

add_subdirectory (map)
add_subdirectory (vatsim)