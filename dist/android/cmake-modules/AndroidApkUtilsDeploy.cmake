include (CMakeParseArguments)
include (${CMAKE_SOURCE_DIR}/AndroidApkUtilsConfig.cmake)

function (android_find_dependencies output)
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


if (EXISTS ${binary_root}/cmake_install.cmake)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${package_location} -P ${binary_root}/cmake_install.cmake
    )
endif ()

android_find_dependencies(deps)
foreach (d ${deps})
    find_library (d_location_${d} ${d}
        PATHS ${qt_libs_dir} ${paths}
        NO_DEFAULT_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH
    )
    
    if (d_location_${d})
        file (INSTALL ${d_location_${d}}
            DESTINATION ${package_location}/libs/${ANDROID_ABI}
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

# gnustl_shared is a must-have of built as a shared library
file (INSTALL ${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/4.9/libs/${ANDROID_ABI}/libgnustl_shared.so
    DESTINATION ${package_location}/libs/${ANDROID_ABI}
)
list (INSERT qt_libs 0 "<item>gnustl_shared</item>")

# install plugins
foreach (p ${qt_plugins})
    set (p_absolute ${qt_plugins_dir}/${p})
    file (RELATIVE_PATH p_location ${qt_root} ${p_absolute})
    
    string (REPLACE "/" "_" p_ext "${p_location}")
    set (p_ext "lib${p_ext}")
    
    file (INSTALL
        DESTINATION ${package_location}/libs/${ANDROID_ABI}
        TYPE FILE RENAME ${p_ext}
        FILES ${p_absolute}
    )
    
    list (APPEND bundled_in_lib "<item>${p_ext}:${p_location}</item>")
endforeach ()

# install QML modules
foreach (m ${qt_qml_modules})
    set (m_location ${qt_qmls_dir}/${m})
    file (GLOB_RECURSE qml_files RELATIVE ${qt_root} ${m_location}/*)
    foreach (f ${qml_files})
        set (f_absolute ${qt_root}/${f})
        
        if (${f} MATCHES "\\.so$")
            get_filename_component (f_name ${f} NAME)
            string (REPLACE "/" "_" f_ext "${f}")
            set (f_ext "lib${f_ext}")
            
            file (INSTALL
                DESTINATION ${package_location}/libs/${ANDROID_ABI}
                TYPE FILE RENAME ${f_ext}
                FILES ${f_absolute}
            )
            
            list (APPEND bundled_in_lib "<item>${f_ext}:${f}</item>")
        else ()
            get_filename_component (f_dir ${f} DIRECTORY)
            file (COPY ${f_absolute} NO_SOURCE_PERMISSIONS
                DESTINATION ${package_location}/assets/${assets_prefix}/${f_dir}
            )
            
            list (APPEND bundled_in_assets "<item>${assets_prefix}/${f}:${f}</item>")
        endif ()
    endforeach ()
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

execute_process (COMMAND ${ant_bin} ${ant_args}
    WORKING_DIRECTORY ${package_location}
)

file (COPY ${ant_output_file} DESTINATION ${binary_root})
