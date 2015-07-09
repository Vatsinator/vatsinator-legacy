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
find_program(ANT_BIN NAMES ant CMAKE_FIND_ROOT_PATH_BOTH)
if (NOT ANT_BIN)
    message(FATAL_ERROR "Could not find ant. Please install ant and add it to "
        "the search path or provide CMake with the ANT_BIN variable directly.")
endif ()

find_program(ANDROID_BIN NAMES android android.bat CMAKE_FIND_ROOT_PATH_BOTH)
if (NOT ANDROID_BIN)
    message(FATAL_ERROR "Could not find 'Android SDK and AVD Manager'. Please "
        "install the android sdk and add the <android-sdk>/tools directory to "
        "the search path or provide CMake with the ANDROID_BIN variable directly.")
endif ()

find_program(ADB_BIN NAMES adb adb.exe CMAKE_FIND_ROOT_PATH_BOTH)
if (NOT ADB_BIN)
    message(WARNING "Could not find adb. Add it to the path or provide CMake "
        "with the ADB_BIN variable if you want to use the install target")
endif ()

if (Qt5Core_FOUND)
    get_target_property (_qmake_bin Qt5::qmake IMPORTED_LOCATION)
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
    platforms/libqeglfs.so
    platforms/libqminimal.so
    platforms/libqminimalegl.so
    platforms/libqoffscreen.so
    generic/libqevdevtabletplugin.so
    generic/libqevdevtouchplugin.so
)

# a list of necessary jars
set (_android_musthave_jars
    QtAndroid-bundled.jar
    QtAndroidAccessibility-bundled.jar
)

set(_android_apkutils_dir ${CMAKE_CURRENT_LIST_DIR})
include(CMakeParseArguments)

function (_android_generate_config target)
    cmake_policy (PUSH)
    cmake_policy (SET CMP0026 OLD)
    get_target_property (target_location ${target} LOCATION)
    cmake_policy (POP)
    
    set (libs_xml_location ${_android_apkutils_dir}/libs.xml.in)
    set (files_dir ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkFiles)
    
    configure_file (${_android_apkutils_dir}/AndroidApkUtilsConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkUtilsConfig.cmake @ONLY)
endfunction ()

#
#       android_deploy_apk(<target>
#               [RESOURCES resources...]
#               [SOURCES sources...]
#               [PATHS paths...]
#               [QT_QML_MODULES modules...]
#               [QT_PLUGINS plugins...]
#               [QT_GENERATE_LIBS_XML 0|1]
#               [ASSETS_PREFIX prefix])
#
# Copies resources, java sources, finds dependencies, installs QML modules,
# plugins and builds the apk package.
#
# This function requries the ANDROID_MANIFEST property of the given target to
# define the path to the AndroidManifest.xml file.
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
# modules are listed as directories, relative to the qml/ directory in Qt root.
#
# QT_PLUGINS is a list of Qt plugins to be shipped with the apk package,
# relative to the plugins/ directory in the Qt root. Note that all necessary
# plugins (like qtforandroid) are deployed automatically.
#
# The QT_GENERATE_LIBS_XML is enabled by default. If enabled, the libs.xml
# file will be generated and put in res/values/ directory in the apk. It is
# necessary for Qt-based apps.
#
# ASSETS_PREFIX defines the prefix for files installed by AndroidApkUtils in
# the assets/ directory. androiddeployqt uses "--Added-by-androiddeployqt--",
# AndroidApkUtils uses "--Added-by-AndroidApkUtils--" by default.
#
function (android_deploy_apk target)
    cmake_parse_arguments (
        _arg
        ""
        "ASSETS_PREFIX"
        "PATHS;RESOURCES;SOURCES;QT_QML_MODULES;QT_PLUGINS;QT_GENERATE_LIBS_XML"
        ${ARGN}
    )
    
    get_target_property (manifest_file ${target} ANDROID_MANIFEST)
    if (NOT manifest_file)
        message (FATAL_ERROR "AndroidManifest.xml not created!")
    endif ()
    
    get_target_property (package_name ${target} ANDROID_PACKAGE_NAME)
    if (NOT package_name)
        message (FATAL_ERROR "Android package name not set! Please update the ANDROID_PACKAGE_NAME property for target ${target}")
    endif ()
    
    set (package_location ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkFiles/${package_name})
    
    add_custom_target (android_refresh_package
        DEPENDS ${target}
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${package_location}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${package_location}
    )
    
    # copy package resources
    set (resources_to_copy "${_qt_root}/src/android/java/res")
    if (_arg_RESOURCES)
        foreach (r ${_arg_RESOURCES})
            list (APPEND resources_to_copy ${r})
        endforeach ()
    endif ()
    
    foreach (r ${resources_to_copy})
        list (
            APPEND resources_copy_cmd
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${r} ${package_location}/res/
        )
    endforeach ()
    
    add_custom_target (android_copy_resources
        DEPENDS android_refresh_package
        ${resources_copy_cmd}
    )
    
    # copy java sources
    set (sources_to_copy "${_qt_root}/src/android/java/src")
    if (_arg_SOURCES)
        foreach (s ${_arg_SOURCES})
            list (APPEND sources_to_copy ${s})
        endforeach ()
    endif ()
    
    foreach (s ${sources_to_copy})
        list (
            APPEND sources_copy_cmd
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${s} ${package_location}/src/
        )
    endforeach ()
    
    add_custom_target (android_copy_sources
        DEPENDS android_refresh_package
        ${sources_copy_cmd}
    )
    
    add_custom_command (
        OUTPUT build.xml
        DEPENDS android_copy_resources android_copy_sources
        COMMAND ${CMAKE_COMMAND} -E copy ${manifest_file} ${package_location}
        COMMAND ${ANDROID_BIN} update project -t android-${ANDROID_NATIVE_API_LEVEL} --name ${CMAKE_PROJECT_NAME} -p ${package_location}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkFiles
    )
    
    if (_arg_PATHS)
        set (paths ${_arg_PATHS})
    endif ()
    
    set (qt_generate_libs_xml 1)
    if (NOT "${_arg_QT_GENERATE_LIBS_XML}" STREQUAL "")
        if ("${_arg_QT_GENERATE_LIBS_XML}" MATCHES "^[01]$")
            set (qt_generate_libs_xml ${_arg_QT_GENERATE_LIBS_XML})
         endif ()
    endif ()
    
    if (_arg_QT_QML_MODULES)
        set (qt_qml_modules ${_arg_QT_QML_MODULES})
    endif ()
    
    set (qt_plugins ${_android_musthave_plugins})
    if (_arg_QT_PLUGINS)
        foreach (p ${_arg_QT_PLUGINS})
            list (APPEND qt_plugins ${p})
        endforeach ()
    endif ()
    list (REMOVE_DUPLICATES qt_plugins)
    
    set (qt_jars ${_android_musthave_jars})
    list (REMOVE_DUPLICATES qt_jars)
    
    set (assets_prefix "--Added-by-AndroidApkUtils--")
    if (_arg_ASSETS_PREFIX)
        set (assets_prefix ${_arg_ASSETS_PREFIX})
    endif ()
    
    _android_generate_config(${target})
    configure_file (${_android_apkutils_dir}/AndroidApkUtilsDeploy.cmake ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkUtilsDeploy.cmake COPYONLY)
    
    add_custom_target (apk
            ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkUtilsDeploy.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS build.xml
    )
    
    string (TOLOWER ${CMAKE_PROJECT_NAME} app_name)
    set (package_file_name ${app_name}-debug.apk) # TODO support app signing
    
    add_custom_target (apk_install
        DEPENDS apk
        COMMAND ${ADB_BIN} install -r ${package_file_name}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
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
