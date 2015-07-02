set (ANDROID_PACKAGE_NAME "org.eu.vatsinator.vatsinator")

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

# find sdk utils
find_program(ANT_BIN NAMES ant)
if (NOT ANT_BIN)
    message(FATAL_ERROR "Could not find ant. Please install ant and add it to the search path.")
endif ()

find_program(ANDROID_BIN NAMES android android.bat)
if (NOT ANDROID_BIN)
    message(FATAL_ERROR "Could not find 'Android SDK and AVD Manager'. Please install the android sdk and add the <android-sdk>/tools directory to the search path.")
endif ()

find_program(ADB_BIN NAMES adb adb.exe)
if (NOT ADB_BIN)
    message(WARNING "Could not find adb. Add it to the path if you want to use the deploy/run_apk targets")
endif ()

get_target_property(QtCore_location Qt5::Core LOCATION)
get_target_property(QtGui_location Qt5::Gui LOCATION)
get_target_property(QtNetwork_location Qt5::Network LOCATION)
get_target_property(QtQuick_location Qt5::Quick LOCATION)
get_target_property(QtQml_location Qt5::Qml LOCATION)
get_target_property(QtAndroidExtras_location Qt5::AndroidExtras LOCATION)

install (FILES
        ${QtCore_location}
        ${QtGui_location}
        ${QtNetwork_location}
        ${QtQuick_location}
        ${QtQml_location}
        ${QtAndroidExtras_location}
        DESTINATION libs/${ANDROID_ABI}
    )

get_filename_component (_quazip_fname ${QUAZIP_LIBRARIES} NAME)
get_filename_component (_quazip_realpath ${QUAZIP_LIBRARIES} REALPATH)

install (FILES ${_quazip_realpath} DESTINATION libs/${ANDROID_ABI} RENAME ${_quazip_fname})

include (QtAndroidDeploy)

# setup permissions
set (ANDROID_PERMISSIONS
    ACCESS_NETWORK_STATE
    INTERNET
)

set (ANDROID_PERMISSIONS_XML "")
foreach (PERMISSION ${ANDROID_PERMISSIONS})
    set (ANDROID_PERMISSIONS_XML "${ANDROID_PERMISSIONS_XML}\n\t<uses-permission android:name=\"android.permission.${PERMISSION}\" />")
endforeach (PERMISSION ${ANDROID_PERMISSIONS})

file (READ ${CMAKE_CURRENT_SOURCE_DIR}/VERSION_CODE ANDROID_VERSION_CODE)
string (REPLACE "\n" "" ANDROID_VERSION_CODE ${ANDROID_VERSION_CODE})
set (ANDROID_VERSION_NAME ${vatsinator_VERSION})

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/AndroidManifest.xml.in ${CMAKE_CURRENT_BINARY_DIR}/AndroidManifest.xml @ONLY)

add_custom_command (
    OUTPUT build.xml
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/AndroidManifest.xml
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/res
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/res ${CMAKE_CURRENT_BINARY_DIR}/res/
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/src
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}/src/
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/source/android/java/* ${CMAKE_CURRENT_BINARY_DIR}/src/*
    COMMAND ${ANDROID_BIN} ARGS update project -t android-${ANDROID_NATIVE_API_LEVEL} --name ${CMAKE_PROJECT_NAME} -p ${CMAKE_CURRENT_BINARY_DIR}
)

get_property (vatsinator_LIBRARY TARGET vatsinator PROPERTY LOCATION)

# install required plugins
install_android_qt_plugins (
    platforms/android/libqtforandroid.so
    platforms/libqeglfs.so
    platforms/libqminimal.so
    platforms/libqminimalegl.so
    platforms/libqoffscreen.so
    generic/libqevdevkeyboardplugin.so
    generic/libqevdevmouseplugin.so
    generic/libqevdevtabletplugin.so
    generic/libqevdevtouchplugin.so
    qmltooling/libqmldbg_qtquick2.so
    qmltooling/libqmldbg_tcp.so
)

install_android_qt_qmls (
    Qt/labs/folderlistmodel/libqmlfolderlistmodelplugin.so
    Qt/labs/settings/libqmlsettingsplugin.so
    QtGraphicalEffects/
    QtQml/Models.2/libmodelsplugin.so
    QtQuick/Controls/
    QtQuick/Dialogs/libdialogplugin.so
    QtQuick/Dialogs/Private/libdialogsprivateplugin.so
    QtQuick/Layouts/libqquicklayoutsplugin.so
    QtQuick/Window.2/libwindowplugin.so
    QtQuick.2/libqtquick2plugin.so
    
)

# list of bundled jars
install_android_qt_jars(
    QtAndroid-bundled.jar
    QtAndroidAccessibility-bundled.jar
)

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/libs.xml.in ${CMAKE_CURRENT_BINARY_DIR}/res/values/libs.xml @ONLY)

if (${ANDROID_STL} MATCHES "gnustl*")
    install (FILES ${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/4.9/libs/${ANDROID_ABI}/libgnustl_shared.so
        DESTINATION libs/${ANDROID_ABI})
endif (${ANDROID_STL} MATCHES "gnustl*")

add_custom_target (android_resources
    DEPENDS build.xml vatsinator
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/libs/${ANDROID_ABI}
    COMMAND ${CMAKE_COMMAND} -E copy ${vatsinator_LIBRARY} ${CMAKE_CURRENT_BINARY_DIR}/libs/${ANDROID_ABI}/
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${vatsinator_ANDROID_APK}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${vatsinator_ANDROID_APK}/libs ${CMAKE_CURRENT_BINARY_DIR}/libs
)

if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    set (ANT_ARGS "debug")
    set (ANDROID_APK_FILE_NAME vatsinator-debug.apk)
elseif (${CMAKE_BUILD_TYPE} MATCHES Release) # TODO support app signing
    set (ANT_ARGS "debug")
    set (ANDROID_APK_FILE_NAME vatsinator-debug.apk)
endif (${CMAKE_BUILD_TYPE} MATCHES Debug)

add_custom_target (build_apk
    DEPENDS android_resources
    COMMAND ${ANT_BIN} ${ANT_ARGS}
)

add_custom_target (deploy_apk
    DEPENDS build_apk
    COMMAND ${ADB_BIN} uninstall ${ANDROID_PACKAGE_NAME}
    COMMAND ${ADB_BIN} install -r ${CMAKE_CURRENT_BINARY_DIR}/bin/${ANDROID_APK_FILE_NAME}
)
