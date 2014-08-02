set (ANDROID_PACKAGE_NAME "org.eu.vatsinator.vatsinator")

# find sdk utils
find_program(ANT_BIN NAMES ant)
if (NOT ANT_BIN)
  message(FATAL_ERROR "Could not find ant. Please install ant and add it to the search path.")
endif (NOT ANT_BIN)

find_program(ANDROID_BIN NAMES android android.bat)
if (NOT ANDROID_BIN)
  message(FATAL_ERROR "Could not find 'Android SDK and AVD Manager'. Please install the android sdk and add the <android-sdk>/tools directory to the search path.")
endif (NOT ANDROID_BIN)

find_program(ADB_BIN NAMES adb adb.exe)
if (NOT ADB_BIN)
  message(WARNING "Could not find adb. Add it to the path if you want to use the deploy/run_apk targets")
endif (NOT ADB_BIN)


# setup permissions
set (ANDROID_PERMISSIONS
  ACCESS_NETWORK_STATE
  INTERNET
  READ_EXTERNAL_STORAGE
  WRITE_EXTERNAL_STORAGE
  SET_DEBUG_APP
)

set (ANDROID_PERMISSIONS_XML "")
foreach (PERMISSION ${ANDROID_PERMISSIONS})
  set (ANDROID_PERMISSIONS_XML "${ANDROID_PERMISSIONS_XML}
\t<uses-permission android:name=\"android.permission.${PERMISSION}\" />")

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
  COMMAND ${ANDROID_BIN} ARGS update project -t android-${ANDROID_NATIVE_API_LEVEL} --name ${CMAKE_PROJECT_NAME} -p ${CMAKE_CURRENT_BINARY_DIR}
)

get_property (vatsinator_LIBRARY TARGET ${vatsinator_TARGET} PROPERTY LOCATION)

# list qt plugins
if (NOT QT_QMAKE_EXECUTABLE)
  get_target_property (QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
endif (NOT QT_QMAKE_EXECUTABLE)
exec_program (${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_PLUGINS"
    OUTPUT_VARIABLE QT_PLUGINS_DIR)

# see res/values/libs.xml
set (ANDROID_QT_PLUGINS
  platforms/android/libqtforandroid.so
  platforms/libqeglfs.so
  platforms/libqminimal.so
  platforms/libqminimalegl.so
  platforms/libqoffscreen.so
  generic/libqevdevkeyboardplugin.so
  generic/libqevdevmouseplugin.so
  generic/libqevdevtabletplugin.so
  generic/libqevdevtouchplugin.so
  accessible/libqtaccessiblewidgets.so
)

foreach (PLUGIN ${ANDROID_QT_PLUGINS})
  install (FILES ${QT_PLUGINS_DIR}/${PLUGIN}
    DESTINATION libs/${ANDROID_ABI})
endforeach (PLUGIN ${ANDROID_QT_PLUGINS})

# list of bundled jars
exec_program (${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_PREFIX"
    OUTPUT_VARIABLE QT_PREFIX)

set (ANDROID_QT_JARS
  QtAndroid-bundled.jar
  QtAndroidAccessibility-bundled.jar
)

foreach (JAR ${ANDROID_QT_JARS})
  install (FILES ${QT_PREFIX}/jar/${JAR}
    DESTINATION libs)
endforeach (JAR ${ANDROID_QT_JARS})

if (${ANDROID_STL} MATCHES "gnustl*")
  install (FILES ${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/4.8/libs/${ANDROID_ABI}/libgnustl_shared.so
    DESTINATION libs/${ANDROID_ABI})
endif (${ANDROID_STL} MATCHES "gnustl*")

add_custom_target (android_resources
  DEPENDS build.xml ${vatsinator_TARGET}
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
elseif (${CMAKE_BUILD_TYPE} MATCHES Release)
  set (ANT_ARGS "release")
endif (${CMAKE_BUILD_TYPE} MATCHES Debug)

add_custom_target (build_apk
  DEPENDS android_resources
  COMMAND ${ANT_BIN} ${ANT_ARGS}
)

add_custom_target (deploy_apk
  DEPENDS build_apk
  COMMAND ${ADB_BIN} uninstall ${ANDROID_PACKAGE_NAME}
  COMMAND ${ADB_BIN} install -r ${CMAKE_CURRENT_BINARY_DIR}/bin/vatsinator-debug.apk # TODO apk name for release
)

add_custom_target (run_apk
  COMMAND ${ADB_BIN} shell am start -n ${ANDROID_PACKAGE_NAME}/org.qtproject.qt5.android.bindings.QtActivity
)
        
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
  # 1. generate a fake Android.mk
  file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/jni/Android.mk "APP_ABI := ${ANDROID_ABI}\n")

  # 2. generate gdb.setup
  get_directory_property (INCLUDE_DIRECTORIES DIRECTORY . INCLUDE_DIRECTORIES)
  string (REGEX REPLACE ";" " " INCLUDE_DIRECTORIES "${INCLUDE_DIRECTORIES}")
  file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/libs/${ANDROID_ABI}/gdb.setup "set solib-search-path ${CMAKE_CURRENT_BINARY_DIR}/obj/local/${ANDROID_ABI}\n")
  file (APPEND ${CMAKE_CURRENT_BINARY_DIR}/libs/${ANDROID_ABI}/gdb.setup "directory ${INCLUDE_DIRECTORIES}\n")

  # 3. copy gdbserver executable
  file (COPY ${ANDROID_NDK}/prebuilt/android-arm/gdbserver/gdbserver DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/libs/${ANDROID_ABI}/)
endif (${CMAKE_BUILD_TYPE} MATCHES Debug)
