/*
 * pluginfinder.cpp
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

#include "pluginfinder.h"
#include <QtCore>

namespace Vatsinator { namespace Misc {

PluginFinder::PluginFinder(const QString &directory) :
    PluginFinder(QStringList({ directory }))
{

}

PluginFinder::PluginFinder(const QStringList &directories)
{
    locatePlugins(directories);
}

QList<QString> PluginFinder::pluginsForIid(const QString& iid)
{
    QList<QString> names;
    for (const PluginData& p: qAsConst(m_plugins)) {
        if (p.iid == iid)
            names.append(p.id);
    }

    return names;
}

QJsonObject PluginFinder::pluginMetaData(const QString& id)
{
    auto it = std::find_if(m_plugins.begin(), m_plugins.end(), [&id](auto it) {
        return it.id == id;
    });

    return it->metaData;
}

QObject* PluginFinder::plugin(const QString& id)
{
    auto it = std::find_if(m_plugins.begin(), m_plugins.end(), [&id](auto it) {
        return it.id == id;
    });

    if (it == m_plugins.end()) {
        qWarning("Plugin \"%s\" not found", qPrintable(id));
        return nullptr;
    }

    if (!m_loadedPlugins.contains(it->fileName)) {
        if (it->metaData.value("dependencies").isArray()) {
            QJsonArray deps = it->metaData.value("dependencies").toArray();
            for (const QJsonValue& val: qAsConst(deps)) {
                QObject* d = this->plugin(val.toString());
                if (!d) {
                    qWarning("Failed loading %s (failed loading dependency: %s)", qPrintable(it->id), qPrintable(val.toString()));
                    return nullptr;
                }
            }
        }

        bool result = loadPlugin(it->fileName);
        if (!result)
            return nullptr;
    }

    return m_loadedPlugins.value(it->fileName);
}

void PluginFinder::locatePlugins(const QStringList& directories)
{
    for (const QString& loc: qAsConst(directories)) {
        QDir dir(loc);
        QStringList files = dir.entryList(QDir::Files);
        for (const QString& file: qAsConst(files))
            readPlugin(dir.absoluteFilePath(file));
    }
}

void PluginFinder::readPlugin(const QString &fileName)
{
    if (QLibrary::isLibrary(fileName)) {
        QPluginLoader loader(fileName);
        QJsonObject metaData = loader.metaData();
        if (metaData.value("IID").isUndefined()) {
            qWarning("Error loading %s: IID is undefined", qPrintable(fileName));
            return;
        }

        QString iid = metaData["IID"].toString();
        QString name = metaData["className"].toString();
        QJsonObject metaData2 = metaData["MetaData"].toObject();

        qDebug("Plugin found in %s (%s implements %s)", qPrintable(fileName),
               qPrintable(name), qPrintable(iid));

        m_plugins.append({name, fileName, iid, metaData2});
    }
}

bool PluginFinder::loadPlugin(const QString &fileName)
{
    QPluginLoader loader(fileName);
    QObject* instance = loader.instance();
    if (!instance) {
        qWarning("Error loading %s (error message: %s)", qPrintable(fileName), qPrintable(loader.errorString()));
        return false;
    } else {
        m_loadedPlugins.insert(fileName, instance);
        qDebug("Loaded %s (%s)", qPrintable(fileName), qPrintable(instance->metaObject()->className()));
        return true;
    }
}

}} /* namespace Vatsinator::Misc */
