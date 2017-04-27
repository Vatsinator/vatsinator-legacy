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

namespace Vatsinator { namespace Core {

class __VtrCoreHide__ PluginFinderHelper {
public:
    void locatePlugins() {
        QStringList locations = { QCoreApplication::applicationDirPath() % "/plugins" };

#ifdef Q_OS_WIN32
# warning "Add plugin locations for Win32"
#endif

#ifdef Q_OS_MACOS
# warning "Add plugin locations for macOS"
#endif

#ifdef Q_OS_ANDROID
        /* This is a hacky way, but I couldn't find better */
        QDir dir(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first());
        dir.cdUp();
        locations.append(dir.absolutePath() % "/qt-reserved-files/plugins");
#endif

        for (const QString& loc: qAsConst(locations)) {
            QDir dir(loc);
            QStringList files = dir.entryList(QDir::Files);
            for (const QString& file: qAsConst(files))
                PluginFinder::readPlugin(dir.absoluteFilePath(file));
        }
    }
};

QList<QString> PluginFinder::pluginsForIid(const QString& iid)
{
    QList<QString> names;
    for (const PluginData& p: qAsConst(m_plugins)) {
        if (p.iid == iid)
            names.append(p.className);
    }

    return names;
}

QJsonObject PluginFinder::pluginMetaData(const QString& className)
{
    auto it = std::find_if(m_plugins.begin(), m_plugins.end(), [&className](auto it) {
        return it.className == className;
    });

    return it->metaData;
}

QObject* PluginFinder::plugin(const QString& className)
{
    auto it = std::find_if(m_plugins.begin(), m_plugins.end(), [&className](auto it) {
        return it.className == className;
    });

    if (!m_loadedPlugins.contains(it->fileName)) {
        bool result = loadPlugin(it->fileName);
        if (!result)
            return nullptr;
    }

    return m_loadedPlugins.value(it->fileName);
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

QList<PluginFinder::PluginData> PluginFinder::m_plugins;
QMap<QString, QObject*> PluginFinder::m_loadedPlugins;

}} /* namespace Vatsinator::Core */


static void initialize()
{
    Vatsinator::Core::PluginFinderHelper p;
    p.locatePlugins();
}
Q_COREAPP_STARTUP_FUNCTION(initialize)
