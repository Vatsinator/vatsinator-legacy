set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)
include (AndroidApkUtils)

file (READ ${CMAKE_CURRENT_SOURCE_DIR}/VERSION_CODE ANDROID_VERSION_CODE)
string (REPLACE "\n" "" ANDROID_VERSION_CODE ${ANDROID_VERSION_CODE})


set_target_properties (vatsinator PROPERTIES
    ANDROID_APP_NAME "Vatsinator"
    ANDROID_VERSION_CODE ${ANDROID_VERSION_CODE}
    ANDROID_VERSION_NAME ${vatsinator_VERSION}
    ANDROID_PACKAGE_NAME "org.eu.vatsinator.vatsinator"
)

android_generate_manifest(vatsinator
    ANDROID_ACTIVITY_CLASS "org.eu.vatsinator.VatsinatorActivity"
    ANDROID_PERMISSIONS ACCESS_NETWORK_STATE INTERNET
    TEMPLATE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/AndroidManifest.xml.in
)

get_filename_component (quazip_path ${QUAZIP_LIBRARIES} DIRECTORY)

android_deploy_apk(vatsinator
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
    QML_PLUGINS
        mapplugin
        vatsimplugin
)
