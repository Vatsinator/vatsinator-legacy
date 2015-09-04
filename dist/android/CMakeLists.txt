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

option (SIGN_APK "Build the apk signed" OFF)
if (SIGN_APK)
    set (KEYSTORE ${CMAKE_CURRENT_BINARY_DIR}
        CACHE FILEPATH "Path to the KeyStore file")
    set (KEYSTORE_ALIAS "mykey"
        CACHE STRING "KeyStore key entry")
    set (KEYSTORE_PASSWORD ""
        CACHE STRING "KeyStore password")

    if (KEYSTORE AND KEYSTORE_ALIAS AND KEYSTORE_PASSWORD)
        set_target_properties (vatsinator PROPERTIES
            KEYSTORE ${KEYSTORE}
            KEYSTORE_ALIAS ${KEYSTORE_ALIAS}
            KEYSTORE_PASSWORD ${KEYSTORE_PASSWORD}
        )
    else ()
        message (WARNING "Keystore not set up despite enabling SIGN_APK option")
    endif ()
endif ()

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
