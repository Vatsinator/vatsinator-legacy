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

#ifndef MISC_PLUGINFINDER_H
#define MISC_PLUGINFINDER_H

#include "misc/vtrmisc_export.h"
#include <QtCore/QObject>
#include <QtCore/QJsonObject>
#include <QtCore/QList>
#include <QtCore/QMap>

namespace Vatsinator { namespace Misc {

/**
 * \ingroup Core
 * @{
 */
class VTRMISC_EXPORT PluginFinder {
    friend class PluginFinderHelper;

public:
    explicit PluginFinder(const QString& directory);

    explicit PluginFinder(const QStringList& directories);

    /**
     * Returns list of plugins that implement the given IID.
     * Each string is a unique id of a plugin.
     */
    QList<QString> pluginsForIid(const QString& iid);

    /**
     * Returns MetaData of the given plugin.
     */
    QJsonObject pluginMetaData(const QString& id);
    
    /**
     * Returns a plugin instance for the given plugin id.
     */
    QObject* plugin(const QString& id);
    
private:
    void locatePlugins(const QStringList& directories);
    void readPlugin(const QString& fileName);
    bool loadPlugin(const QString& fileName);
    
    struct PluginData {
        QString className;
        QString id;
        QString fileName;
        QString iid;
        QJsonObject metaData;
    };

    QList<PluginData> m_plugins;
    QMap<QString, QObject*> m_loadedPlugins; /**< fileName <-> instance */

}; /** @} */

}} /* namespace Vatsinator::Misc */

Q_DECLARE_METATYPE(Vatsinator::Misc::PluginFinder*)

#endif // MISC_PLUGINFINDER_H
