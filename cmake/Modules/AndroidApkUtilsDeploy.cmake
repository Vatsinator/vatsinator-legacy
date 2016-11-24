include (CMakeParseArguments)
include (${CMAKE_SOURCE_DIR}/AndroidApkUtilsConfig.cmake)

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


# GLobal variables:
# bundled_in_lib
# bundled_in_assets
# qt_libs
# bundled_libs


# Finds target dependencies using the readelf util.
function (android_find_target_dependencies target output)
    if (NOT EXISTS ${target})
        message (FATAL_ERROR "Target file: ${target} does not exist!")
    endif ()
    
    execute_process(
        COMMAND ${readelf_bin} ${target} -d -W
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

# Install provided libraries, add them to lists
function (android_deploy_libs libs qt_libs_output bundled_libs_output)
    foreach (lib ${libs})
        find_library (lib_location_${lib} ${lib}
            PATHS
                ${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/4.9/libs/${ANDROID_ABI}
                ${qt_libs_dir}
                ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
                ${paths}
            NO_DEFAULT_PATH
            NO_CMAKE_ENVIRONMENT_PATH
            NO_SYSTEM_ENVIRONMENT_PATH
            NO_CMAKE_SYSTEM_PATH
        )
        
        if (lib_location_${lib})
            file (COPY ${lib_location_${lib}}
                DESTINATION ${package_location}/libs/${ANDROID_ABI}
                NO_SOURCE_PERMISSIONS
            )
            
            string (REGEX
                REPLACE "^lib(.+)\\.so$"
                "\\1"
                lib_name "${lib}"
            )
            
            if (${lib_name} MATCHES "^(Qt5)|(gnustl_shared).*")
                list (APPEND qt_libs "${lib_name}")
            else ()
                list (APPEND bundled_libs "${lib_name}")
            endif ()
        else ()
            message (WARNING "Could not satisfy dependency: ${lib}")
        endif ()
    endforeach ()
    
    set (${qt_libs_output} ${qt_libs} PARENT_SCOPE)
    set (${bundled_libs_output} ${bundled_libs} PARENT_SCOPE)
endfunction ()

# Deploy a single, custom plugin
function (android_deploy_plugin plugin)
    if (NOT IS_ABSOLUTE ${plugin})
        message (WARNING "${plugin} is not an absolute path; cannot deploy it")
    else ()
        file (RELATIVE_PATH p_relative ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} ${plugin})
        string (REPLACE "/" "_" p_ext "${p_relative}")
        set (p_ext "lib${p_ext}")
        
        set (p_destination ${package_location}/libs/${ANDROID_ABI})
        file (COPY ${plugin}
            DESTINATION ${p_destination}
            NO_SOURCE_PERMISSIONS
        )
        
        get_filename_component (p_name ${plugin} NAME)
        file (RENAME
            ${p_destination}/${p_name}
            ${p_destination}/${p_ext}
        )
        
        list (APPEND bundled_in_lib "${p_ext}:${p_relative}")
    endif ()
    
    set (bundled_in_lib ${bundled_in_lib} PARENT_SCOPE)
endfunction ()

# plugin_dir - target directory of the plugin (i.e. "QtQuick/Controls/Styles")
# plugin_source_dir - source directory of the plugin, absolute path (i.e. "/opt/Qt/5.5/android_armv7")
# plugin_files - list of files to be deployed (qmldir, .qml, .so), paths relative to plugin_source
# plugin prefix - where all the files (except libraries) will be deployed; this dir has to be in the QML import paths
function (android_deploy_qml_plugin plugin_dir plugin_source_dir plugin_files plugin_prefix)
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
            
            list (APPEND bundled_in_lib "${fname_ext}:${plugin_prefix}/${plugin_dir}/${file}")
            
            android_find_target_dependencies (${file_absolute} plugin_deps)
            android_deploy_libs ("${plugin_deps}" qt_libs bundled_libs)
        else ()
            get_filename_component (file_target_dir ${file} DIRECTORY)
            file (COPY ${file_absolute}
                DESTINATION ${package_location}/assets/${assets_prefix}/${plugin_prefix}/${plugin_dir}/${file_target_dir}
                NO_SOURCE_PERMISSIONS
            )
            list (APPEND bundled_in_assets "${assets_prefix}/${plugin_prefix}/${plugin_dir}/${file}:${plugin_prefix}/${plugin_dir}/${file}")
        endif ()
    endforeach ()
    
    set (bundled_in_lib ${bundled_in_lib} PARENT_SCOPE)
    set (bundled_in_assets ${bundled_in_assets} PARENT_SCOPE)
    set (qt_libs ${qt_libs} PARENT_SCOPE)
    set (bundled_libs ${bundled_libs} PARENT_SCOPE)
endfunction ()

# Wraps each line with the <item> xml tag
function (items_wrap_xml list output)
    foreach (item ${list})
        list (APPEND _output "<item>${item}</item>")
    endforeach ()
    
    set (${output} ${_output} PARENT_SCOPE)
endfunction ()

# Enables target for remote debugging
function (enable_debug)
     # 1. generate essential Android Makefiles
    file (MAKE_DIRECTORY ${package_location}/jni)
    if (NOT EXISTS ${package_location}/jni/Android.mk)
        file (WRITE ${package_location}/jni/Android.mk "APP_ABI := ${ANDROID_ABI}\n")
    endif ()
    
    if (NOT EXISTS ${package_location}/jni/Application.mk)
        file (WRITE ${package_location}/jni/Application.mk "APP_ABI := ${ANDROID_ABI}\n")
    endif ()

    # 2. generate gdb.setup
    file (WRITE ${package_location}/libs/${ANDROID_ABI}/gdb.setup "set solib-search-path ${package_location}/libs/${ANDROID_ABI}\n")
    file (APPEND ${package_location}/libs/${ANDROID_ABI}/gdb.setup "directory ${includes}\n")

    # 3. copy gdbserver executable
    file (COPY ${ANDROID_NDK}/prebuilt/android-${ANDROID_ARCH_NAME}/gdbserver/gdbserver DESTINATION ${package_location}/libs/${ANDROID_ABI})
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
android_find_target_dependencies (${target_location} deps)
android_deploy_libs ("${deps}" qt_libs bundled_libs)

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
    file (RENAME ${p_destination}/${fname} ${p_destination}/${p_ext})
    list (APPEND bundled_in_lib "${p_ext}:${p_location}")
    
    android_find_target_dependencies (${p_absolute} plugin_deps)
    android_deploy_libs ("${plugin_deps}" qt_libs bundled_libs)
endforeach ()

# install QML modules
message (STATUS "Installing Qt QML modules")
foreach (m ${qt_qml_modules})
    set (m_location ${qt_qmls_dir}/${m})
    file (GLOB_RECURSE qml_files RELATIVE ${m_location} ${m_location}/*)
    
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
    
    if (${p}_files)
        foreach (f ${${p}_files})
            list (APPEND p_files ${f})
        endforeach ()
    endif ()
    
    get_filename_component (p_qmldir_fname ${p_qmldir} NAME)
    list (APPEND p_files ${p_qmldir_fname})
    android_deploy_qml_plugin (${p_dir} ${p_path} "${p_files}" ${p_prefix})
    unset (p_files)
endforeach ()

# install custom plugins
message (STATUS "Installing custom plugins")
if (plugins)
    foreach (p ${plugins})
        android_deploy_plugin (${p})
    endforeach ()
endif ()

if (qt_libs)
    list (REMOVE_DUPLICATES qt_libs)
endif ()

if (bundled_libs)
    list (REMOVE_DUPLICATES bundled_libs)
endif ()

# install Qt's jars
foreach (j ${qt_jars})
    file (COPY ${qt_root}/jar/${j}
        DESTINATION ${package_location}/libs
    )
endforeach ()

if (qt_generate_libs_xml)
    items_wrap_xml ("${qt_libs}" qt_libs)
    string (REPLACE ";" "\n" qt_libs "${qt_libs}")
    items_wrap_xml ("${bundled_libs}" bundled_libs)
    string (REPLACE ";" "\n" bundled_libs "${bundled_libs}")
    items_wrap_xml ("${bundled_in_lib}" bundled_in_lib)
    string (REPLACE ";" "\n" bundled_in_lib "${bundled_in_lib}")
    items_wrap_xml ("${bundled_in_assets}" bundled_in_assets)
    string (REPLACE ";" "\n" bundled_in_assets "${bundled_in_assets}")
    configure_file (${libs_xml_location} ${package_location}/res/values/libs.xml @ONLY)
endif ()

# enable remote debugging
if (CMAKE_BUILD_TYPE MATCHES Debug)
    enable_debug ()
endif ()

# build the actual .apk package
string (TOLOWER ${CMAKE_PROJECT_NAME} app_name)

if (keystore AND keystore_alias AND keystore_password)
    set (ant_args "release")
    set (ant_output_file ${package_location}/bin/${app_name}-release.apk)
    
    file (WRITE ${package_location}/ant.properties
        "key.store=${keystore}\nkey.alias=${keystore_alias}\nkey.store.password=${keystore_password}\nkey.alias.password=${keystore_password}"
    )
else ()
    set (ant_args "debug")
    set (ant_output_file ${package_location}/bin/${app_name}-debug.apk)
endif ()

get_filename_component (pkg_fname ${ant_output_file} NAME)
message (STATUS "Building ${pkg_fname}...")

set (ant_log_file ${files_dir}/ant.log)

execute_process (COMMAND ${ant_bin} ${ant_args}
    WORKING_DIRECTORY ${package_location}
    RESULT_VARIABLE ant_result
    OUTPUT_FILE ${ant_log_file}
)

if (EXISTS ${package_location}/ant.properties)
    file (REMOVE ${package_location}/ant.properties)
endif ()

if (${ant_result} EQUAL 0)
    file (COPY ${ant_output_file} DESTINATION ${binary_current})
    message ("${pkg_fname} done.")
else ()
    message (FATAL_ERROR "Failed building APK package; see ant logs in ${ant_log_file}")
endif ()
