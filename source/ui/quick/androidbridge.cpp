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

#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <jni.h>

#include "androidbridge.h"

AndroidBridge::AndroidBridge(QObject* parent) :
    QObject(parent),
    __navigationBarColor(Qt::black)
{
    connect(this, &AndroidBridge::navigatinBarColorChanged, this, &AndroidBridge::__setNavigationBarColorImpl);
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
AndroidBridge::resetNavigationBarColor()
{
   setNavigationBarColor(QColor(Qt::black));
}

void
AndroidBridge::__setNavigationBarColorImpl(const QColor& navigationBarColor)
{
    QAndroidJniObject::callStaticMethod<void>(
        "org/eu/vatsinator/VatsinatorActivity",
        "setNavigationBarColor",
        "(III)V", // gets three ints, returns void
        navigationBarColor.red(), navigationBarColor.green(), navigationBarColor.blue()
    );
    
    QAndroidJniEnvironment env;
    if (env->ExceptionCheck()) {
        qWarning("Could not set navigation bar color; the exception follows...");
        env->ExceptionDescribe();
        env->ExceptionClear();
    }
}
