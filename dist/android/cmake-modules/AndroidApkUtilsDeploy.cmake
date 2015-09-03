include (CMakeParseArguments)
include (${CMAKE_SOURCE_DIR}/AndroidApkUtilsConfig.cmake)

# GLobal variables:
# bundled_in_lib
# bundled_in_assets
# qt_libs
# bundled_libs

# Finds target dependencies using the readelf util.
function (android_find_target_dependencies output)
    if (NOT EXISTS ${target_location})
        message (FATAL_ERROR "Target file: ${target_location} does not exist!")
    endif ()

    # Find all preinstalled libraries
    # TODO Support other architectures too
    set (android_platform_dir ${ANDROID_NDK}/platforms/android-${ANDROID_NATIVE_API_LEVEL}/arch-arm/usr/lib)
    file (GLOB_RECURSE android_preinstalled
        RELATIVE ${android_platform_dir}
        "${android_platform_dir}/*.so"
    )
    
    # TODO Support non-arms
    find_program (readelf_bin
        NAMES arm-linux-androideabi-readelf arm-linux-androideabi-readelf.exe
        PATHS ${ANDROID_TOOLCHAIN_ROOT}
        PATH_SUFFIXES bin
        CMAKE_FIND_ROOT_PATH_BOTH
    )
    
    if (NOT readelf_bin)
        message (FATAL_ERROR "Could not find readelf!")
    endif ()
    
    execute_process(
        COMMAND ${readelf_bin} ${target_location} -d -W
        RESULT_VARIABLE readelf_result
        OUTPUT_VARIABLE readelf_output
    )
    
    if (${readelf_result} EQUAL 0)
        string (REPLACE "\n" ";" readelf_output_lines ${readelf_output})
        foreach (line ${readelf_output_lines})
            string (REGEX
                REPLACE "^.+\\(NEEDED\\).+\\[(.+)\\]"
                "\\1"
                libname "${line}"
            )
            
            if ("${libname}" MATCHES ".so$")
                # look if the given dependency is one of the preinstalled libraries on Android
                list (FIND android_preinstalled ${libname} is_found)
                if (${is_found} EQUAL -1)
                    list (APPEND _output ${libname})
                endif ()
            endif ()
        endforeach ()
        
        set (${output} ${_output} PARENT_SCOPE)
    else ()
        message (FATAL_ERROR "Could not execute ${readelf_bin}!")
    endif ()
endfunction ()

# plugin_dir - target directory of the plugin (i.e. "QtQuick/Controls/Styles")
# plugin_source_dir - source directory of the plugin, absolute path (i.e. "/opt/Qt/5.5/android_armv7")
# plugin_files - list of files to be deployed (qmldir, .qml, .so), paths relative to plugin_source
# plugin prefix - where all the files (except libraries) will be deployed; this dir has to be in the QML import paths
function (android_deploy_qml_plugin plugin_dir plugin_source_dir plugin_files plugin_prefix)
    if (NOT plugin_dir)
        message(FATAL_ERROR "No plugin directory specified")
    endif ()
    
    foreach (file ${plugin_files})
        if (IS_ABSOLUTE ${file})
            set (file_absolute ${file})
            get_filename_component (file ${file} NAME)
        else ()
            set (file_absolute ${plugin_source_dir}/${file})
        endif ()
        
        if (${file} MATCHES "\\.so$")
            set (fname_ext ${plugin_dir}/${file})
            string (REPLACE "/" "_" fname_ext "${fname_ext}")
            set (fname_ext "lib${fname_ext}")
            
            message ("${plugin_dir}/${file} => ${fname_ext}")
            
            set (f_destination ${package_location}/libs/${ANDROID_ABI})
            
            file (COPY ${file_absolute}
                DESTINATION ${f_destination}
                NO_SOURCE_PERMISSIONS
            )
            
            get_filename_component (fname ${file} NAME)
            file (RENAME
                ${f_destination}/${fname}
                ${f_destination}/${fname_ext}
            )
            
            list (APPEND bundled_in_lib "<item>${fname_ext}:${plugin_prefix}/${plugin_dir}/${file}</item>")
        else ()
            get_filename_component (file_target_dir ${file} DIRECTORY)
            file (COPY ${file_absolute}
                DESTINATION ${package_location}/assets/${assets_prefix}/${plugin_prefix}/${plugin_dir}/${file_target_dir}
                NO_SOURCE_PERMISSIONS
            )
            list (APPEND bundled_in_assets "<item>${assets_prefix}/${plugin_prefix}/${plugin_dir}/${file}:${plugin_prefix}/${plugin_dir}/${file}</item>")
        endif ()
    endforeach ()
    
    set (bundled_in_lib ${bundled_in_lib} PARENT_SCOPE)
    set (bundled_in_assets ${bundled_in_assets} PARENT_SCOPE)
endfunction ()


# first, install all user stuff
message (STATUS "Executing make install")
if (EXISTS ${binary_root}/cmake_install.cmake)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${package_location} -P ${binary_root}/cmake_install.cmake
        OUTPUT_QUIET
    )
endif ()

# second, install dependencies
message (STATUS "Installing dependencies")
android_find_target_dependencies(deps)
foreach (d ${deps})
    find_library (d_location_${d} ${d}
        PATHS ${qt_libs_dir} ${paths}
        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH
    )
    
    if (d_location_${d})
        file (COPY ${d_location_${d}}
            DESTINATION ${package_location}/libs/${ANDROID_ABI}
            NO_SOURCE_PERMISSIONS
        )
        
        string (REGEX
            REPLACE "^lib(.+)\\.so$"
            "\\1"
            libname "${d}"
        )
        
        if (${libname} MATCHES "^Qt5.*")
            list (APPEND qt_libs "<item>${libname}</item>")
        else ()
            list (APPEND bundled_libs "<item>${libname}</item>")
        endif ()
    endif ()
endforeach ()

# gnustl_shared is a must-have, as Qt libraries need it
message (STATUS "Installing libgnustl_shared.so")
file (COPY ${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/4.9/libs/${ANDROID_ABI}/libgnustl_shared.so
    DESTINATION ${package_location}/libs/${ANDROID_ABI}
    NO_SOURCE_PERMISSIONS
)
list (INSERT qt_libs 0 "<item>gnustl_shared</item>")

# install plugins
message (STATUS "Installing Qt plugins")
foreach (p ${qt_plugins})
    set (p_absolute ${qt_plugins_dir}/${p})
    file (RELATIVE_PATH p_location ${qt_root} ${p_absolute})
    
    string (REPLACE "/" "_" p_ext "${p_location}")
    set (p_ext "lib${p_ext}")
    
    set (p_destination ${package_location}/libs/${ANDROID_ABI})
    
    file (COPY ${p_absolute}
        DESTINATION ${p_destination}
        NO_SOURCE_PERMISSIONS
    )
    
    get_filename_component (fname ${p} NAME)
    message ("${p} => ${p_ext}")
    file (RENAME ${p_destination}/${fname} ${p_destination}/${p_ext})
    list (APPEND bundled_in_lib "<item>${p_ext}:${p_location}</item>")
endforeach ()

# install QML modules
message (STATUS "Installing Qt QML modules")
foreach (m ${qt_qml_modules})
    set (m_location ${qt_qmls_dir}/${m})
    file (GLOB_RECURSE qml_files RELATIVE ${m_location} ${m_location}/*)
    #file (GLOB_RECURSE qml_files ${m_location}/*)
    
    foreach (f ${qml_files})
        list (APPEND m_files ${f})
    endforeach ()
    
    android_deploy_qml_plugin (${m} ${m_location} "${m_files}" "qml")
    unset (m_files)
endforeach ()

# install custom QML plugins
message (STATUS "Installing QML plugins")
foreach (p ${qml_plugins})
    set (p_dir ${${p}_dir})
    set (p_qmldir ${${p}_qmldir})
    set (p_prefix ${${p}_prefix})
    get_filename_component (p_path ${p_qmldir} DIRECTORY)
    
    if (${p}_location)
        list (APPEND p_files ${${p}_location})
    endif ()
    
    if (${p}_qmls)
        foreach (q ${${p}_qmls})
            list (APPEND p_files ${q})
        endforeach ()
    endif ()
    
    get_filename_component (p_qmldir_fname ${p_qmldir} NAME)
    list (APPEND p_files ${p_qmldir_fname})
    android_deploy_qml_plugin (${p_dir} ${p_path} "${p_files}" ${p_prefix})
    unset (p_files)
endforeach ()

# install Qt's jars
foreach (j ${qt_jars})
    file (COPY ${qt_root}/jar/${j}
        DESTINATION ${package_location}/libs
    )
endforeach ()

if (qt_generate_libs_xml)
    string (REPLACE ";" "\n" qt_libs "${qt_libs}")
    string (REPLACE ";" "\n" bundled_libs "${bundled_libs}")
    string (REPLACE ";" "\n" bundled_in_lib "${bundled_in_lib}")
    string (REPLACE ";" "\n" bundled_in_assets "${bundled_in_assets}")
    configure_file (${libs_xml_location} ${package_location}/res/values/libs.xml @ONLY)
endif ()



# build the actual .apk package
string (TOLOWER ${CMAKE_PROJECT_NAME} app_name)

if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    set (ant_args "debug")
    set (ant_output_file ${package_location}/bin/${app_name}-debug.apk)
elseif (${CMAKE_BUILD_TYPE} MATCHES Release) # TODO support app signing
    set (ant_args "debug")
    set (ant_output_file ${package_location}/bin/${app_name}-debug.apk)
endif (${CMAKE_BUILD_TYPE} MATCHES Debug)

get_filename_component (pkg_fname ${ant_output_file} NAME)
message (STATUS "Building ${pkg_fname}...")

set (ant_log_file ${files_dir}/ant.log)

execute_process (COMMAND ${ant_bin} ${ant_args}
    WORKING_DIRECTORY ${package_location}
    RESULT_VARIABLE ant_result
    OUTPUT_FILE ${ant_log_file}
)

if (${ant_result} EQUAL 0)
    file (COPY ${ant_output_file} DESTINATION ${binary_root})
    message ("${pkg_fname} done.")
else ()
    message (FATAL_ERROR "Failed building APK package; see ant logs in ${ant_log_file}")
endif ()
