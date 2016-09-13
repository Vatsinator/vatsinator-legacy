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

auto PluginFinderPropKey = "pluginFinderInstance";

namespace Vatsinator { namespace Core {

PluginFinder::PluginFinder(QObject* parent) : QObject(parent)
{
    locatePlugins();
}

QList<QObject*> PluginFinder::pluginsForIid(const QString& iid)
{
    PluginFinder* instance = qApp->property(PluginFinderPropKey).value<PluginFinder*>();
    Q_ASSERT(instance);
    return instance->pluginsForIidImpl(iid);
}

QObject* PluginFinder::pluginByName(const QString& name)
{
    PluginFinder* instance = qApp->property(PluginFinderPropKey).value<PluginFinder*>();
    Q_ASSERT(instance);
    return instance->pluginByNameImpl(name);
}

void PluginFinder::locatePlugins()
{
    QStringList locations = { QCoreApplication::applicationDirPath() % "/plugins" };

#ifdef Q_OS_WIN32
    locations.append(QDir::cleanPath(QCoreApplication::applicationDirPath() % "/../plugins"));
#endif

#if defined(Q_CC_MSVC) && !defined(QT_NO_DEBUG)
    locations.append(QDir::cleanPath(QCoreApplication::applicationDirPath() % "/../plugins/Debug"));
#endif
    
#ifdef Q_OS_ANDROID
    /* This is a hacky way, but I couldn't find better */
    QDir dir(QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first());
    dir.cdUp();
    locations.append(dir.absolutePath() % "/qt-reserved-files/plugins");
#endif
    
    std::for_each(locations.begin(), locations.end(), [this](auto path) {
        QDir dir(path);
        QStringList files = dir.entryList(QDir::Files);
        std::for_each(files.begin(), files.end(), [this, &dir](auto file) {
            this->readPlugin(dir.absoluteFilePath(file));
        });
    });
}

void PluginFinder::readPlugin(const QString& fileName)
{
    if (QLibrary::isLibrary(fileName)) {
        QPluginLoader loader(fileName);
        QJsonObject metaData = loader.metaData();
        if (metaData.value("IID").isUndefined()) {
            qWarning("Error loading %s: IID is undefined", qPrintable(fileName));
            return;
        }
        
        QString iid = metaData["IID"].toString();
        m_pluginsByIid.insertMulti(iid, fileName);
        
        QString className = metaData["className"].toString();
        m_pluginsByName.insert(className, fileName);
        
        qDebug("Plugin found in %s (%s implements %s)", qPrintable(fileName),
               qPrintable(className), qPrintable(iid));
    }
}

QList<QObject*> PluginFinder::pluginsForIidImpl(const QString& iid)
{
    QList<QObject*> plugins;
    auto fileNames = m_pluginsByIid.values(iid);
    std::for_each(fileNames.begin(), fileNames.end(), [this, &plugins](auto fileName) {
        plugins.append(this->getPlugin(fileName));
    });
    
    return plugins;
}

QObject* PluginFinder::pluginByNameImpl(const QString& name)
{
    QString fileName = m_pluginsByName.value(name);
    if (fileName.isEmpty()) {
        qWarning("Failed loading %s: no such plugin", qPrintable(name));
        return nullptr;
    }
    
    return getPlugin(fileName);
}

QObject* PluginFinder::getPlugin(const QString& fileName)
{
    if (!m_instances.contains(fileName)) {
        bool result = this->loadPlugin(fileName);
        if (!result)
            return nullptr;
    }
    
    return m_instances.value(fileName);
}

bool PluginFinder::loadPlugin(const QString& fileName)
{
    QPluginLoader loader(fileName);
    QObject* instance = loader.instance();
    if (!instance) {
        qWarning("Error loading %s (error message: %s)", qPrintable(fileName), qPrintable(loader.errorString()));
        return false;
    } else {
        m_instances.insert(fileName, instance);
        qDebug("Loaded %s (%s)", qPrintable(fileName), qPrintable(instance->metaObject()->className()));
        return true;
    }
}

}} /* namespace Vatsinator::Core */


static void initialize()
{
    using namespace Vatsinator::Core;
    
    PluginFinder* pf = new PluginFinder(qApp);
    qApp->setProperty(PluginFinderPropKey, QVariant::fromValue(pf));
}
Q_COREAPP_STARTUP_FUNCTION(initialize)
