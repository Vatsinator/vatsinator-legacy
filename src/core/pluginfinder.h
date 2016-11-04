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

#include "coreexport.h"
#include <QObject>
#include <QMap>

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * @{
 */
class __VtrCoreApi__ PluginFinder : public QObject {
    Q_OBJECT

public:
    /**
     * \internal
     */
    explicit PluginFinder(QObject* parent = nullptr);
    
    /**
     * Returns list of plugins that implement the given interface ID.
     */
    static QList<QObject*> pluginsForIid(const QString& iid);
    
    /**
     * Returns a plugin for the given class name.
     */
    static QObject* pluginByName(const QString& name);
    
private:
    /**
     * Locates all the plugins.
     */
    void locatePlugins();
    
    /**
     * Reads the plugin's metadata.
     */
    void readPlugin(const QString& fileName);
    
    QList<QObject*> pluginsForIidImpl(const QString& iid);
    QObject* pluginByNameImpl(const QString& name);
    QObject* getPlugin(const QString& fileName);
    bool loadPlugin(const QString& fileName);
    
    QMap<QString, QString> m_pluginsByIid; /**< IID <-> file name */
    QMap<QString, QString> m_pluginsByName; /**< class name <-> file name */
    QMap<QString, QObject*> m_instances; /**< file name <-> instance */

}; /** @} */

}} /* namespace Vatsinator::Core */

Q_DECLARE_METATYPE(Vatsinator::Core::PluginFinder*)

#endif // CORE_PLUGINFINDER_H
