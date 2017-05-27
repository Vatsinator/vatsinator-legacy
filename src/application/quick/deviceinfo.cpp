#include "deviceinfo.h"
#include <QtAndroidExtras/QAndroidJniEnvironment>
#include <QtAndroid>
#include <QtGui>

DeviceInfo::DeviceInfo(QObject *parent) : QObject(parent)
{

}

int DeviceInfo::navigationBarHeight()
{
    if (m_navigationBarHeight < 0) {
        QAndroidJniObject activity = QtAndroid::androidActivity();
        if (!activity.isValid()) {
            qWarning("No activity at this time; navigation bar height could not be fetched");
            return 0;
        }

        /* public int getNavigationBarHeight() */
        jint h = activity.callMethod<jint>("getNavigationBarHeight", "()I");

        QAndroidJniEnvironment env;
        if (env->ExceptionCheck()) {
            qWarning("Could not get the navigation bar height; the exception follows...");
            env->ExceptionDescribe();
            env->ExceptionClear();
            return 0;
        }

        m_navigationBarHeight = static_cast<int>(h) / qApp->primaryScreen()->devicePixelRatio();
    }

    return m_navigationBarHeight;
}
