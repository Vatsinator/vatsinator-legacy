/*
 * androidbridge.cpp
 * Copyright (C) 2016  Michał Garapich <michal@garapich.pl>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include "androidbridge.h"
#include <QtAndroid>
#include <QAndroidJniEnvironment>
#include <QtGui>

namespace Vatsinator { namespace Imports {

AndroidBridge::AndroidBridge(QObject* parent) :
    QObject(parent)
{
    onScreenOrientationChanged();
    connect(qApp->primaryScreen(), &QScreen::orientationChanged, this, &AndroidBridge::onScreenOrientationChanged);
}

void AndroidBridge::fetchNavigationBarHeight()
{
    QAndroidJniObject activity = QtAndroid::androidActivity();
    if (!activity.isValid()) {
        qWarning("No activity at this time; navigation bar height could not be fetched");
        return;
    }
    
    /* public int getNavigationBarHeight() */
    jint h = activity.callMethod<jint>("getNavigationBarHeight", "()I");
    
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        qWarning("Could not get the navigation bar height; the exception follows...");
        env->ExceptionDescribe();
        env->ExceptionClear();
        return;
    }
    
    m_navigationBarHeight = static_cast<int>(h);
}

void AndroidBridge::onScreenOrientationChanged()
{
    fetchNavigationBarHeight();
}

}} /* namespace Vatsinator::Imports */
