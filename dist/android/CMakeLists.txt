set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
include (QtAndroidDeploy)
include (AndroidApkUtils)

file (READ ${CMAKE_CURRENT_SOURCE_DIR}/VERSION_CODE ANDROID_VERSION_CODE)
string (REPLACE "\n" "" ANDROID_VERSION_CODE ${ANDROID_VERSION_CODE})

set (ANDROID_PACKAGE_NAME "org.eu.vatsinator.vatsinator")

android_generate_manifest(${CMAKE_CURRENT_BINARY_DIR}/AndroidManifest.xml
    ANDROID_APP_NAME "Vatsinator"
    ANDROID_APP_LIB "vatsinator"
    ANDROID_PACKAGE_NAME ${ANDROID_PACKAGE_NAME}
    ANDROID_VERSION_CODE ${ANDROID_VERSION_CODE}
    ANDROID_VERSION_NAME ${vatsinator_VERSION}
    ANDROID_ACTIVITY_CLASS "org.eu.vatsinator.VatsinatorActivity"
    ANDROID_PERMISSIONS ACCESS_NETWORK_STATE INTERNET
    TEMPLATE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/AndroidManifest.xml.in
)

get_filename_component (quazip_path ${QUAZIP_LIBRARIES} DIRECTORY)

android_deploy_apk(vatsinator
    ANDROID_APP_NAME "Vatsinator"
    ANDROID_PACKAGE_NAME ${ANDROID_PACKAGE_NAME}
    ANDROID_MANIFEST ${CMAKE_CURRENT_BINARY_DIR}/AndroidManifest.xml
    RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/res
    SOURCES ${PROJECT_SOURCE_DIR}/source/android/java
    PATHS ${quazip_path}
    QT_QML_MODULES
        QtGraphicalEffects
        QtQuick/Controls
        QtQuick/Dialogs
        QtQuick/Layouts
        QtQuick/Window.2
        QtQuick.2
    QT_GENERATE_LIBS_XML
)
