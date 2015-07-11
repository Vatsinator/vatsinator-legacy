/*
 * androidbridge.cpp
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
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

#include <QtAndroid>
#include <QAndroidJniEnvironment>
#include <jni.h>

#include "androidbridge.h"

AndroidBridge::AndroidBridge(QObject* parent) :
    QObject(parent),
    __navigationBarColor(Qt::black)
{
    connect(this, &AndroidBridge::navigatinBarColorChanged, this, &AndroidBridge::__setNavigationBarColorImpl);
    connect(this, &AndroidBridge::statusBarColorChanged, this, &AndroidBridge::__setStatusBarColorImpl);
}

void
AndroidBridge::setNavigationBarColor(const QColor& color)
{
    if (__navigationBarColor != color) {
        __navigationBarColor = color;
        emit navigatinBarColorChanged(__navigationBarColor);
    }
}

void
AndroidBridge::setStatusBarColor(const QColor& color)
{
    if (__statusBarColor != color) {
        __statusBarColor = color;
        emit statusBarColorChanged(__statusBarColor);
    }
}

void
AndroidBridge::resetNavigationBarColor()
{
   setNavigationBarColor(QColor(Qt::black));
}

void
AndroidBridge::__setNavigationBarColorImpl(const QColor& navigationBarColor)
{
    QAndroidJniObject activity = QtAndroid::androidActivity();
    if (!activity.isValid()) {
        qWarning("No activity at this time; not setting navigation bar color");
        return;
    }
    
    /* public void setNavigationBarColor(int r, int g, int b, int a) */
    activity.callMethod<void>(
        "setNavigationBarColor",
        "(IIII)V", // gets four ints, returns void
        navigationBarColor.red(), navigationBarColor.green(), navigationBarColor.blue(), navigationBarColor.alpha()
    );
    
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        qWarning("Could not set navigation bar color; the exception follows...");
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}

void
AndroidBridge::__setStatusBarColorImpl(const QColor& statusBarColor)
{
    QAndroidJniObject activity = QtAndroid::androidActivity();
    if (!activity.isValid()) {
        qWarning("No activity at this time; not setting status bar color");
        return;
    }
    
    /* public void setStatusBarColor(int r, int g, int b, int a) */
    activity.callMethod<void>(
        "setStatusBarColor",
        "(IIII)V",
        statusBarColor.red(), statusBarColor.green(), statusBarColor.blue(), statusBarColor.alpha()
    );
    
    
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        qWarning("Could not set status bar color; the exception follows...");
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}
