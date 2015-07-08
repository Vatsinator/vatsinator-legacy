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
    
    configure_file (${_android_apkutils_dir}/AndroidApkUtilsConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkUtilsConfig.cmake @ONLY)
endfunction ()

#
#       android_deploy_apk(<target>
#               ANDROID_PACKAGE_NAME package_name
#               ANDROID_MANIFEST manifest_file
#               [RESOURCES resources...]
#               [SOURCES sources...]
#               [PATHS paths...]
#               [QT_QML_MODULES modules...]
#               [QT_PLUGINS plugins...]
#               [QT_GENERATE_LIBS_XML]
#               [ASSETS_PREFIX prefix])
#
# Builds the APK package.
#
function (android_deploy_apk target)
    cmake_parse_arguments (
        _arg
        "QT_GENERATE_LIBS_XML"
        "ANDROID_PACKAGE_NAME;ANDROID_MANIFEST;ASSETS_PREFIX"
        "PATHS;RESOURCES;SOURCES;QT_QML_MODULES;QT_PLUGINS"
        ${ARGN}
    )
    
    if (NOT _arg_ANDROID_MANIFEST)
        message (FATAL_ERROR "AndroidManifest.xml location not specified!")
    endif ()
    set (manifest_file ${_arg_ANDROID_MANIFEST})
    
    if (NOT _arg_ANDROID_PACKAGE_NAME)
        message (FATAL_ERROR "Android package name not set! Please provide the ANDROID_PACKAGE_NAME argument")
    endif ()
    set (package_name ${_arg_ANDROID_PACKAGE_NAME})
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
    )
    
    if (_arg_PATHS)
        set (paths ${_arg_PATHS})
    endif ()
    
    if (_arg_QT_GENERATE_LIBS_XML)
        set (qt_generate_libs_xml 1)
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
    
    add_custom_target (android_deploy_apk ALL
            ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/AndroidApkUtilsDeploy.cmake
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS build.xml
    )
    
    string (TOLOWER ${CMAKE_PROJECT_NAME} app_name)
    set (package_file_name ${app_name}-debug.apk)
    
    add_custom_target (apk_install
        DEPENDS ${target}
        COMMAND ${ADB_BIN} install -r ${package_file_name}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endfunction ()

#
#       android_generate_manifest(<output>
#               ANDROID_APP_NAME app_name
#               ANDROID_APP_LIB app_lib
#               ANDROID_PACKAGE_NAME package_name
#               ANDROID_VERSION_CODE version_code
#               ANDROID_VERSION_NAME version_name
#               [TEMPLATE_FILE template_file]
#               [ANDROID_APPLICATION_CLASS application_class]
#               [ANDROID_ACTIVITY_CLASS activity_class]
#               [ANDROID_PERMISSIONS permissions...]
#               [QT_DONT_BUNDLE_LOCAL_QT_LIBS])
#               
# Generates the AndroidManifest.xml file from the given template. The template
# is taken from Qt sources and is designed to support Qt applications. It is
# however possible to provide any custom template.
#
# The ANDROID_APP_NAME is the app name itself.
#
# The ANDROID_APP_LIB is the main library of the app.
#
# The ANDROID_PACKAGE_NAME specifies the fully-qualified package name for the
# Android app, i.e. "org.foo.bar.baz".
#
# The ANDROID_VERSION_CODE defines version code for the Android app.
#
# The ANDROID_VERSION_NAME is the human-readable app version.
#
# The TEMPLATE_FILE is a path to the AndroidManifest.xml template file. Each of
# variables documented here can be re-used in the template, with "@" syntax.
# If not specified, the Qt's default template is used.
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
# The QT_DONT_BUNDLE_LOCAL_QT_LIBS specified whether the Qt libs should be bundled with the
# APK or not. This value is not set by default, so the APK does not require Ministro.
#
# Sample usage:
# android_qt_generate_manifest(${CMAKE_CURRENT_BINARY_DIR}/AndroidManifest.xml
#               ANDROID_APP_NAME "Baz"
#               ANDROID_APP_LIB "baz"
#               ANDROID_PACKAGE_NAME "org.foo.bar.baz"
#               ANDROID_VERSION_CODE 2
#               ANDROID_VERSION_NAME "1.1"
#               ANDROID_ACTIVITY_CLASS "org.foo.bar.BazActivity"
#               ANDROID_PERMISSIONS ACCESS_NETWORK_STATE INTERNET RECEIVE_SMS)
#
# TODO Add ANDROID_FEATURES option to auto-generate features as well
#
function (android_generate_manifest output)
    cmake_parse_arguments (
        _arg
        "QT_DONT_BUNDLE_LOCAL_QT_LIBS"
        "ANDROID_APP_NAME;ANDROID_APP_LIB;ANDROID_PACKAGE_NAME;ANDROID_VERSION_CODE;ANDROID_VERSION_NAME;TEMPLATE_FILE;ANDROID_APPLICATION_CLASS;ANDROID_ACTIVITY_CLASS"
        "ANDROID_PERMISSIONS"
        ${ARGN}
    )
    
    if (NOT _arg_ANDROID_APP_NAME)
        message (FATAL_ERROR "Android app name not set! Please provide the ANDROID_APP_NAME argument")
    endif ()
    set (ANDROID_APP_NAME ${_arg_ANDROID_APP_NAME})
    
    if (NOT _arg_ANDROID_APP_LIB)
        message (FATAL_ERROR "Android app library not set! Please provide the ANDROID_APP_LIB argument")
    endif ()
    set (ANDROID_APP_LIB ${_arg_ANDROID_APP_LIB})
    
    if (NOT _arg_ANDROID_PACKAGE_NAME)
        message (FATAL_ERROR "Android package name not set! Please provide the ANDROID_PACKAGE_NAME argument")
    endif ()
    set (ANDROID_PACKAGE_NAME ${_arg_ANDROID_PACKAGE_NAME})
    
    if (NOT _arg_ANDROID_VERSION_CODE)
        message (FATAL_ERROR "Android app version code not set! Please provide the ANDROID_VERSION_CODE argument")
    endif ()
    set (ANDROID_VERSION_CODE ${_arg_ANDROID_VERSION_CODE})
    
    if (NOT _arg_ANDROID_VERSION_NAME)
        message (FATAL_ERROR "Android app version name not set! Please provide the ANDROID_VERSION_NAME argument")
    endif ()
    set (ANDROID_VERSION_NAME ${_arg_ANDROID_VERSION_NAME})
    
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
    
    if (_arg_QT_DONT_BUNDLE_LOCAL_QT_LIBS)
        set (QT_BUNDLE_LOCAL_QT_LIBS 0)
    else ()
        set (QT_BUNDLE_LOCAL_QT_LIBS 1)
    endif ()
    
    configure_file (${TEMPLATE_FILE} ${output} @ONLY)
endfunction ()
