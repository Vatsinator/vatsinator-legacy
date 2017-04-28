#
#       AndroidApkUtils
#
# Functions to help deploy C and C++ applications for Android with
# some Qt goodies.
# 
# The following functions are provided by this module:
# 
#       android_qt_generate_manifest
#

# find sdk utils
find_program(GRADLE_BIN NAMES gradle)
if (NOT GRADLE_BIN)
    message(FATAL_ERROR "Could not find gradle. Please install gradle and add it to "
        "the search path or provide CMake with the GRADLE_BIN variable directly.")
endif ()

find_program(ADB_BIN NAMES adb)
if (NOT ADB_BIN)
    message(FATAL_ERROR "Could not find adb. Please install adb and add it to "
        "the search path or provide CMake with the ADB_BIN variable directly.")
endif ()

if (Qt5Core_FOUND)
    get_target_property (_qmake_bin Qt5::qmake IMPORTED_LOCATION)
    execute_process(
        COMMAND ${_qmake_bin} -query QT_VERSION
        OUTPUT_VARIABLE _qt_version
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process (
        COMMAND ${_qmake_bin} -query QT_INSTALL_PREFIX
        OUTPUT_VARIABLE _qt_root
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process (
        COMMAND ${_qmake_bin} -query QT_INSTALL_LIBS
        OUTPUT_VARIABLE _qt_libs_dir
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process (
        COMMAND ${_qmake_bin} -query QT_INSTALL_QML
        OUTPUT_VARIABLE _qt_qmls_dir
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process (
        COMMAND ${_qmake_bin} -query QT_INSTALL_PLUGINS
        OUTPUT_VARIABLE _qt_plugins_dir
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif ()

execute_process (
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkFiles
)

# a list of plugins needed for every Android, Qt-based app
set (_android_musthave_plugins
    platforms/android/libqtforandroid.so
)

# a list of necessary jars
set (_android_musthave_jars
    QtAndroid-bundled.jar
)

function (_android_sdk_find_buildtools output_var android_sdk_path)
    file(GLOB build_tools
         LIST_DIRECTORIES true
         RELATIVE ${android_sdk_path}/build-tools
         ${android_sdk_path}/build-tools/[0-9][0-9].[0-9].[0-9]
    )

    list (LENGTH build_tools len)
    if (${len} EQUAL 0)
        message (FATAL_ERROR "No Android build tools found")
    endif ()

    list (SORT build_tools)
    list (GET build_tools -1 latest)
    set (${output_var} ${latest} PARENT_SCOPE)
endfunction ()


set(_android_apkutils_dir ${CMAKE_CURRENT_LIST_DIR})
include(CMakeParseArguments)

#
#       android_deploy_apk(<target>
#               [RESOURCES resources...]
#               [SOURCES sources...]
#               [PATHS paths...]
#               [QT_QML_MODULES modules...]
#               [QT_PLUGINS plugins...]
#               [ASSETS_PREFIX prefix]
#               [QML_PLUGINS targets...]
#               [PLUGINS plugins...])
#
# Copies resources, java sources, finds dependencies, installs QML modules,
# plugins and builds the apk package.
#
# This function requries the ANDROID_MANIFEST property of the given target to
# define the path to the AndroidManifest.xml file. The android_generate_manifest
# target sets the property automatically.
#
# In order to build the apk correctly, path to Android SDK must be set:
# cmake (...) -DANDROID_SDK=/path/to/sdk
#
# The RESOURCES is a list of directories to be copied into res/ subfolder in
# the apk structure.
#
# The SOURCES param is a list of additional Java sources; they will be copied
# into src/ subfolder.
#
# The PATHS is a list of additional paths where the script should be looking
# for dependencies. Qt paths are added by default.
#
# QT_QML_MODULES defines the list of QML modules to be installed; the
# modules are listed as directories, relative to the qml/ directory in the
# Qt root.
#
# QT_PLUGINS is a list of Qt plugins to be shipped with the apk package,
# relative to the plugins/ directory in the Qt root. Note that all necessary
# plugins (like qtforandroid) are deployed automatically.
#
# ASSETS_PREFIX defines the prefix for files installed by AndroidApkUtils in
# the assets/ directory. androiddeployqt uses "--Added-by-androiddeployqt--",
# AndroidApkUtils uses "--Added-by-AndroidApkUtils--" by default.
#
# QML_PLUGINS is a list of targets that are QML plugins. In order to include
# the qmldir file as well, set the QMLDIR_FILE property for each of these
# targets.
#
# PLUGINS is a list of your application plugins to be shipped with the apk.
#
# EXTRA_LIBS is a list of 3rd-party shared libraries to be shipped with
# the apk.
#
function (android_deploy_apk target)
    cmake_parse_arguments (
        _arg
        ""
        "ASSETS_PREFIX"
        "PATHS;RESOURCES;SOURCES;QT_QML_MODULES;QT_PLUGINS;QML_PLUGINS;PLUGINS;EXTRA_LIBS"
        ${ARGN}
    )

    if (NOT ANDROID_SDK)
        message (FATAL_ERROR "ANDROID_SDK not set")
    endif ()

    if (NOT IS_DIRECTORY ${ANDROID_SDK})
        message (FATAL_ERROR "ANDROID_SDK is invalid")
    endif ()
    
    get_target_property (manifest_file ${target} ANDROID_MANIFEST)
    if (NOT manifest_file)
        message (FATAL_ERROR "AndroidManifest.xml not created!")
    endif ()
    
    get_target_property (package_name ${target} ANDROID_PACKAGE_NAME)
    if (NOT package_name)
        message (FATAL_ERROR "Android package name not set! Please update the ANDROID_PACKAGE_NAME property for target ${target}")
    endif ()
    
    set (package_location ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkFiles/${package_name})
    file (MAKE_DIRECTORY ${package_location})

    # copy build files
    configure_file (${manifest_file} ${package_location}/AndroidManifest.xml COPYONLY)
    configure_file (${_android_apkutils_dir}/build.gradle ${package_location}/build.gradle COPYONLY)
    configure_file (${_android_apkutils_dir}/local.properties.in ${package_location}/local.properties)

    set (androidCompileSdkVersion ${CMAKE_SYSTEM_VERSION})
    set (qt5AndroidDir ${_qt_root}/src/android/java)
     _android_sdk_find_buildtools (androidBuildToolsVersion ${ANDROID_SDK})
    string (REGEX REPLACE "^([0-9]+)\..*$" "\\1" androidCompileSdkVersion "${androidBuildToolsVersion}")
    configure_file (${_android_apkutils_dir}/gradle.properties.in ${package_location}/gradle.properties)

    # generate gradle wrapper
    execute_process (
        COMMAND ${GRADLE_BIN} wrapper
        WORKING_DIRECTORY ${package_location}
        OUTPUT_QUIET
    )

    set (gradlew_script ${package_location}/gradlew)
    if (WIN32)
        set (gradlew_script ${gradlew_script}.bat)
    endif ()
    
    # copy package resources
    if (_arg_RESOURCES)
        file (COPY ${_arg_RESOURCES} DESTINATION ${package_location}/res)
    endif ()
    
    # copy java sources
    if (_arg_SOURCES)
        file (COPY ${_arg_SOURCES} DESTINATION ${package_location}/src)
    endif ()

    # configure and copy AndroidApkFiles deploy scripts
    cmake_policy (PUSH)
    cmake_policy (SET CMP0026 OLD)
    get_target_property (target_location ${target} LOCATION)
    cmake_policy (POP)

    set (qt_plugins ${_android_musthave_plugins})
    if (_arg_QT_PLUGINS)
        foreach (p ${_arg_QT_PLUGINS})
            list (APPEND qt_plugins ${p})
        endforeach ()
    endif ()
    list (REMOVE_DUPLICATES qt_plugins)

    if (_arg_QT_QML_MODULES)
            set (qt_qml_modules ${_arg_QT_QML_MODULES})
    endif ()

    set (assets_prefix "--Added-by-AndroidApkUtils--") # used in AndroidApkUtilsConfig
    if (_arg_ASSETS_PREFIX)
        set (assets_prefix ${_arg_ASSETS_PREFIX})
    endif ()

    if (_arg_QML_PLUGINS)
        foreach (p ${_arg_QML_PLUGINS})
            get_target_property (p_dir ${p} PLUGIN_DIR)
            get_target_property (p_qmldir ${p} QMLDIR_FILE)
            get_target_property (p_files ${p} FILES)
            get_target_property (p_prefix ${p} PLUGIN_PREFIX)
            get_filename_component (p_path ${p_qmldir} DIRECTORY)

            cmake_policy (PUSH)
            cmake_policy (SET CMP0026 OLD)
            get_target_property (p_location ${p} LOCATION)
            cmake_policy (POP)

            list (APPEND qml_plugins ${p})

            set (qml_plugins_data "${qml_plugins_data}
                set (${p}_dir \"${p_dir}\")
                set (${p}_qmldir \"${p_qmldir}\")")

            if (p_location)
                set (qml_plugins_data "${qml_plugins_data}
                    set (${p}_location \"${p_location}\")")
            endif ()

            if (p_files)
                set (qml_plugins_data "${qml_plugins_data}
                    set (${p}_files \"${p_files}\")")

                # add to list of all qml files so that apk is dependent on them
                foreach (f ${p_files})
                    list (APPEND all_files ${p_path}/${f})
                endforeach ()
            endif()

            if (p_prefix)
                set (qml_plugins_data "${qml_plugins_data}
                    set (${p}_prefix \"${p_prefix}\")")
            else ()
                set (qml_plugins_data "${qml_plugins_data}
                    set (${p}_prefix \"qml\")")
            endif ()
        endforeach ()
    endif ()

    if (_arg_PLUGINS)
        foreach (p ${_arg_PLUGINS})
            add_dependencies (${target} ${p})

            cmake_policy (PUSH)
            cmake_policy (SET CMP0026 OLD)
            get_target_property (p_location ${p} LOCATION)
            cmake_policy (POP)

            list (APPEND plugins ${p_location})
        endforeach ()
    endif ()

    if (_arg_EXTRA_LIBS)
        foreach (lib ${_arg_EXTRA_LIBS})
            list (APPEND extra_libs ${lib})
        endforeach ()
    endif ()

    set (qt_jars ${_android_musthave_jars}) # used in AndroidApkUtilsConfig
    list (REMOVE_DUPLICATES qt_jars)

    set (libs_xml_location ${_android_apkutils_dir}/libs.xml.in)
    set (files_dir ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkFiles)
    configure_file (${_android_apkutils_dir}/AndroidApkUtilsConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkUtilsConfig.cmake @ONLY)

    configure_file (${_android_apkutils_dir}/AndroidApkUtilsDeploy.cmake ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkUtilsDeploy.cmake COPYONLY)

    set (apk_file_name ${package_name}-debug.apk)

    # builds the apk
    add_custom_command (
        OUTPUT ${apk_file_name}
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkUtilsDeploy.cmake
        DEPENDS
            ${package_location}/AndroidManifest.xml
            ${_arg_QML_PLUGINS} # when any of the qml plugins is rebuilt
            ${_arg_PLUGINS}     # when any of the application plugins is rebuilt
            ${all_files}        # i.e. when qml file is changed
            ${target}           # when the application is rebuilt
    )

    add_custom_target (apk ALL
        DEPENDS ${apk_file_name}
    )

    add_custom_target (apk_install
        ${gradlew_script} installDebug
        WORKING_DIRECTORY ${package_location}
        DEPENDS apk
    )
endfunction ()

#
#       android_generate_manifest(<output>
#               [TEMPLATE_FILE template_file]
#               [ANDROID_APPLICATION_CLASS application_class]
#               [ANDROID_ACTIVITY_CLASS activity_class]
#               [ANDROID_PERMISSIONS permissions...]
#               [QT_BUNDLE_LOCAL_QT_LIBS 0|1])
#               
# Generates the AndroidManifest.xml file from the given template. The template
# is taken from Qt sources and is designed to support Qt applications. It is
# however possible to provide a custom template.
#
# The TEMPLATE_FILE is a path to the AndroidManifest.xml template file. If not
# specified, the Qt's default template is used.
#
# The ANDROID_APPLICATION_CLASS param can be set to define the value of "android:name"
# param in the XML. If not set, "org.qtproject.qt5.android.bindings.QtApplication" is
# used, as it is the default Application implementation for Qt.
#
# The ANDROID_ACTIVITY_CLASS defines the class that implements the main activity.
# If not set, "org.qtproject.qt5.android.bindings.QtActivity" is used.
#
# The ANDROID_PERMISSIONS is a list of Android-specific permissions that will be put
# in the manifest file.
#
# The QT_BUNDLE_LOCAL_QT_LIBS specifies whether the Qt libs should be bundled with the
# APK or not. By default this value is set to 1, meaning that the apk does not
# need Ministro.
#
# As a result, the ANDROID_MANIFEST property on the specified target is set, defining
# the absolute location of the generated AndroidManifest.xml file.
#
# Sample usage:
# android_qt_generate_manifest(baz
#               ANDROID_APP_LIB "baz"
#               ANDROID_ACTIVITY_CLASS "org.foo.bar.BazActivity"
#               ANDROID_PERMISSIONS ACCESS_NETWORK_STATE INTERNET RECEIVE_SMS)
#
# TODO Add ANDROID_FEATURES option to auto-generate features as well
#
function (android_generate_manifest target)
    cmake_parse_arguments (
        _arg
        ""
        "TEMPLATE_FILE;ANDROID_APPLICATION_CLASS;ANDROID_ACTIVITY_CLASS;QT_BUNDLE_LOCAL_QT_LIBS"
        "ANDROID_PERMISSIONS"
        ${ARGN}
    )
    
    get_target_property (ANDROID_APP_NAME ${target} ANDROID_APP_NAME)
    if (NOT ANDROID_APP_NAME)
        message (FATAL_ERROR "Android app name not set! Please update the ANDROID_APP_NAME property for target ${target}")
    endif ()
    
    get_target_property (ANDROID_VERSION_NAME ${target} ANDROID_VERSION_NAME)
    if (NOT ANDROID_VERSION_NAME)
        message (FATAL_ERROR "Android app version name not set! Please update the ANDROID_VERSION_NAME property for target ${target}")
    endif ()
    
    get_target_property (ANDROID_VERSION_CODE ${target} ANDROID_VERSION_CODE)
    if (NOT ANDROID_VERSION_CODE)
        message (FATAL_ERROR "Android app version code not set! Please update the ANDROID_VERSION_CODE property for target ${target}")
    endif ()
    
    get_target_property (ANDROID_PACKAGE_NAME ${target} ANDROID_PACKAGE_NAME)
    if (NOT ANDROID_PACKAGE_NAME)
        message (FATAL_ERROR "Android package name not set! Please update the ANDROID_PACKAGE_NAME property for target ${target}")
    endif ()
    
    set (ANDROID_APP_LIB ${target}) # TODO make this customizable
    
    set (TEMPLATE_FILE "${_android_apkutils_dir}/AndroidManifest.xml.in")
    if (_arg_TEMPLATE_FILE)
        set (TEMPLATE_FILE ${_arg_TEMPLATE_FILE})
    endif ()
    
    set (ANDROID_APPLICATION_CLASS "org.qtproject.qt5.android.bindings.QtApplication")
    if (_arg_ANDROID_APPLICATION_CLASS)
        set (ANDROID_APPLICATION_CLASS ${_arg_ANDROID_APPLICATION_CLASS})
    endif ()
    
    set (ANDROID_ACTIVITY_CLASS "org.qtproject.qt5.android.bindings.QtActivity")
    if (_arg_ANDROID_ACTIVITY_CLASS)
        set (ANDROID_ACTIVITY_CLASS ${_arg_ANDROID_ACTIVITY_CLASS})
    endif ()
    
    set_target_properties (${target} PROPERTIES ANDROID_ACTIVITY_CLASS ${ANDROID_ACTIVITY_CLASS})
    
    if (_arg_ANDROID_PERMISSIONS)
        set (ANDROID_PERMISSIONS "")
        foreach (p ${_arg_ANDROID_PERMISSIONS})
            set (ANDROID_PERMISSIONS "${ANDROID_PERMISSIONS}\n    <uses-permission android:name=\"android.permission.${p}\" />")
        endforeach ()
    endif ()
    
    set (QT_BUNDLE_LOCAL_QT_LIBS 1)
    if (NOT "${_arg_QT_BUNDLE_LOCAL_QT_LIBS}" STREQUAL "")
        if ("${_arg_QT_BUNDLE_LOCAL_QT_LIBS}" MATCHES "^[01]$")
            set (QT_BUNDLE_LOCAL_QT_LIBS ${_arg_QT_BUNDLE_LOCAL_QT_LIBS})
        endif ()
    endif ()
    
    set (output "${CMAKE_CURRENT_BINARY_DIR}/AndroidApkFiles/AndroidManifest.xml")
    configure_file (${TEMPLATE_FILE} ${output} @ONLY)
    set_target_properties (${target} PROPERTIES ANDROID_MANIFEST ${output})
endfunction ()
