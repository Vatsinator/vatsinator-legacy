/*
 * pluginfinder.h
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

#ifndef CORE_PLUGINFINDER_H
#define CORE_PLUGINFINDER_H

#include "core/vtrcore_export.h"
#include <QtCore/QObject>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QMap>

namespace Vatsinator { namespace Core {

class PluginFinderHelper;

/**
 * \ingroup Core
 * @{
 */
class VTRCORE_EXPORT PluginFinder {
    friend class PluginFinderHelper;

public:
    /**
     * Returns list of plugins that implement the given IID.
     * Each string is a unique class name of a plugin.
     */
    static QList<QString> pluginsForIid(const QString& iid);

    /**
     * Returns MetaData of the given plugin.
     */
    static QJsonObject pluginMetaData(const QString& className);
    
    /**
     * Returns a plugin for the given class name.
     */
    static QObject* plugin(const QString& className);

    PluginFinder() = delete;
    
private:
    static void readPlugin(const QString& fileName);
    static bool loadPlugin(const QString& fileName);
    
    struct PluginData {
        QString className;
        QString fileName;
        QString iid;
        QJsonObject metaData;
    };

    static QList<PluginData> m_plugins;
    static QMap<QString, QObject*> m_loadedPlugins; /**< fileName <-> instance */

}; /** @} */

}} /* namespace Vatsinator::Core */

Q_DECLARE_METATYPE(Vatsinator::Core::PluginFinder*)

#endif // CORE_PLUGINFINDER_H
