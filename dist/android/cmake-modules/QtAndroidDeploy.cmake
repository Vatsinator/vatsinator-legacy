#
#       QtAndroidDeploy
#
# Functions to help deploy Qt application for Android.
# 
# The following functions are provided by this module:
# 
#       install_android_qt_plugins
#       install_android_qt_qmls
#       install_android_qt_jars
#

if (NOT QT_QMAKE_EXECUTABLE)
    get_target_property (QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
endif ()

exec_program (${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_PLUGINS"
    OUTPUT_VARIABLE QT_INSTALL_PLUGINS_DIR)
exec_program (${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_QML"
    OUTPUT_VARIABLE QT_INSTALL_QML_DIR)
exec_program (${QT_QMAKE_EXECUTABLE} ARGS "-query QT_INSTALL_PREFIX"
    OUTPUT_VARIABLE QT_INSTALL_PREFIX)

#
#       install_android_qt_plugins(<plugins>)
#
# Installs selected qt plugins (relative to QT_INSTALL_PLUGINS dir) and sets
# QT_ANDROID_XML_bundled_in_lib variable to be later used in libs.xml.in.
#
function (install_android_qt_plugins)
    set (QT_ANDROID_XML_bundled_in_lib "${QT_ANDROID_XML_bundled_in_lib}")
    foreach (PLUGIN ${ARGV})
        install (FILES ${QT_INSTALL_PLUGINS_DIR}/${PLUGIN}
            DESTINATION libs/${ANDROID_ABI})
    
        get_filename_component (PLUGIN_FILE ${PLUGIN} NAME)
        set (QT_ANDROID_XML_bundled_in_lib "${QT_ANDROID_XML_bundled_in_lib}
            <item>${PLUGIN_FILE}:plugins/${PLUGIN}</item>")
    
    endforeach (PLUGIN)
    set (QT_ANDROID_XML_bundled_in_lib "${QT_ANDROID_XML_bundled_in_lib}" PARENT_SCOPE)
endfunction (install_android_qt_plugins)

#
#       install_android_qt_qmls(<qmls>)
#
# Installs selected qml add-ons along with necessary metadata files. All qml libs
# (.so) must be relative to QT_INSTALL_QML. Sets QT_ANDROID_XML_bundled_in_lib
# and QT_ANDROID_XML_bundled_in_assets variables to be later used in libs.xml.in.
#
function (install_android_qt_qmls)
    set (QT_ANDROID_XML_bundled_in_lib "${QT_ANDROID_XML_bundled_in_lib}")
    set (QT_ANDROID_XML_bundled_in_assets "${QT_ANDROID_XML_bundled_in_assets}")
    foreach (QML ${ARGV})
        if (${QML} MATCHES "/$") # directory
            file (GLOB_RECURSE QML_FILES RELATIVE ${QT_INSTALL_QML_DIR} ${QT_INSTALL_QML_DIR}/${QML}*)
            foreach (QML_FILE ${QML_FILES})
                if (${QML_FILE} MATCHES "\\.so$")
                    install_android_qt_qmls(${QML_FILE})
                else ()
                    get_filename_component (QML_FILE_DIR ${QML_FILE} DIRECTORY)
                    install (FILES ${QT_INSTALL_QML_DIR}/${QML_FILE}
                        DESTINATION assets/qml/${QML_FILE_DIR})
                    set (QT_ANDROID_XML_bundled_in_assets "${QT_ANDROID_XML_bundled_in_assets}
                        <item>qml/${QML_FILE}:qml/${QML_FILE}</item>")
                endif ()
            endforeach (QML_FILE)
      
        else () # .so file
            install (FILES ${QT_INSTALL_QML_DIR}/${QML}
                DESTINATION libs/${ANDROID_ABI})
            
            get_filename_component (QML_FILE ${QML} NAME)
            get_filename_component (QML_PATH ${QML} PATH)
            
            set (QT_ANDROID_XML_bundled_in_lib "${QT_ANDROID_XML_bundled_in_lib}
                <item>${QML_FILE}:qml/${QML}</item>")
            
            install (FILES
                ${QT_INSTALL_QML_DIR}/${QML_PATH}/qmldir
                DESTINATION assets/qml/${QML_PATH})
            
            set (QT_ANDROID_XML_bundled_in_assets "${QT_ANDROID_XML_bundled_in_assets}
                <item>qml/${QML_PATH}/qmldir:qml/${QML_PATH}/qmldir</item>")
            
            if (EXISTS ${QT_INSTALL_QML_DIR}/${QML_PATH}/plugins.qmltypes)
                install (FILES
                    ${QT_INSTALL_QML_DIR}/${QML_PATH}/plugins.qmltypes
                    DESTINATION assets/qml/${QML_PATH})
                set (QT_ANDROID_XML_bundled_in_assets "${QT_ANDROID_XML_bundled_in_assets}
                    <item>qml/${QML_PATH}/plugins.qmltypes:qml/${QML_PATH}/plugins.qmltypes</item>")
            endif ()
        endif ()
    endforeach (QML)
    
    set (QT_ANDROID_XML_bundled_in_lib "${QT_ANDROID_XML_bundled_in_lib}" PARENT_SCOPE)
    set (QT_ANDROID_XML_bundled_in_assets "${QT_ANDROID_XML_bundled_in_assets}" PARENT_SCOPE)
endfunction (install_android_qt_qmls)

#
#       install_android_qt_jars(<jars>)
#
# Install selected jar files to the apk bundle. Each jar file must be specified
# relative to ${QT_INSTALL_PREFIX}/jar directory.
#
function (install_android_qt_jars)
    foreach (JAR ${ARGV})
        install (FILES ${QT_INSTALL_PREFIX}/jar/${JAR}
            DESTINATION libs)
    endforeach (JAR)
endfunction (install_android_qt_jars)
