/*
 * nativeutilsqmlplugin.cpp
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

#include "nativeutilsqmlplugin.h"
#include "androidbridge.h"
#include <QtQml>

namespace Vatsinator { namespace Imports {

static QObject* android_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    
    AndroidBridge* android = new AndroidBridge;
    return android;
}

void NativeUtilsQmlPlugin::registerTypes(const char* uri)
{
    Q_ASSERT(uri == QStringLiteral("org.eu.vatsinator.NativeUtils"));
    
    qmlRegisterSingletonType<AndroidBridge>(uri, 1, 0, "Android", &android_singletontype_provider);
}

}} /* namespace Vatsinator::Imports */
