find_package (Qt5 5.7 REQUIRED COMPONENTS Quick Xml AndroidExtras)
include_directories (${Qt5Quick_INCLUDE_DIRS})

find_package (OpenSSL)

set (vatsinator_QUICK_QMLS
    AirportNodeDetails.qml
    AirportDetailsPage.qml
    AtcDetailsPage.qml
    FirDetailsPage.qml
    FlightDetailsPage.qml
    main.qml
)

set (vatsinator_JAVA_SRCS
    android/java/org/eu/vatsinator/VatsinatorActivity.java
)

add_library (vatsinator SHARED
    deviceinfo.cpp
    deviceinfo.h

    main.cpp
    qml.qrc

    ${vatsinator_QUICK_QMLS}
    ${vatsinator_JAVA_SRCS}
)
target_link_libraries (vatsinator
    Qt5::Quick
    Qt5::Xml
    Qt5::AndroidExtras
    vtrgui
)
add_dependencies (vatsinator i18n)
install (TARGETS vatsinator DESTINATION libs/${CMAKE_ANDROID_ARCH_ABI})

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
    ANDROID_PERMISSIONS
        ACCESS_NETWORK_STATE
        INTERNET
    TEMPLATE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/android/AndroidManifest.xml.in
    ANDROID_ACTIVITY_CLASS "org.eu.vatsinator.VatsinatorActivity"
)

android_deploy_apk(vatsinator
    RESOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/android/res/drawable
        ${CMAKE_CURRENT_SOURCE_DIR}/android/res/drawable-ldpi
        ${CMAKE_CURRENT_SOURCE_DIR}/android/res/drawable-mdpi
        ${CMAKE_CURRENT_SOURCE_DIR}/android/res/drawable-hdpi
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/android/java/org
    QT_QML_MODULES
        QtGraphicalEffects
        QtQuick.2
        QtQuick/Controls.2
        QtQuick/Controls.2/Material
        QtQuick/Templates.2
        QtQuick/Layouts
        QtQuick/Window.2
    QML_PLUGINS
        componentsplugin
        vatsimplugin
    QT_PLUGINS
        imageformats/libqsvg.so
    PLUGINS
        euroutenotamprovider
        tiledmapdrawer
    EXTRA_LIBS
        ${OPENSSL_CRYPTO_LIBRARY}
        ${OPENSSL_SSL_LIBRARY}
)
