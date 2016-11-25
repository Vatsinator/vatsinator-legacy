find_package (Qt5AndroidExtras REQUIRED)
include_directories (${Qt5AndroidExtras_INCLUDE_DIRS})

set (nativeutils_SOURCES
    androidbridge.cpp
    nativeutilsqmlplugin.cpp
)

set (nativeutils_HEADERS
    androidbridge.h
    nativeutilsqmlplugin.h
)

add_library (nativeutilsplugin MODULE
    ${nativeutils_SOURCES}
    ${nativeutils_HEADERS}
    qmldir
)

target_link_libraries (nativeutilsplugin Qt5::Qml Qt5::Quick Qt5::AndroidExtras vtrcore)
vatsinator_add_qml_plugin (nativeutilsplugin org/eu/vatsinator/NativeUtils)
