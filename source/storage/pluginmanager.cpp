/*
 * pluginmanager.cpp
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

#include <QtCore>

#include "storage/filemanager.h"

#include "pluginmanager.h"

PluginManager::PluginManager(QObject* parent):
  QObject(parent),
  __initialied(false) {}

QStringList
PluginManager::plugins(const QString& iid) const {
  return __availablePlugins.values(iid);
}

QJsonObject
PluginManager::metadata(const QString& name) const {
  return qMove(__metadata.value(name, QJsonObject()));
}

void
PluginManager::initialize() {
  if (__initialied)
    return;
  
  QDir pluginsDir(FileManager::staticPath(FileManager::Plugins));
  for (QString fileName: pluginsDir.entryList(QDir::Files)) {
    QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
    QJsonObject pluginData = loader.metaData();
    if (!pluginData["IID"].isString()) {
      qWarning("Invalid plugin: %s", qPrintable(loader.fileName()));
      continue;
    }
    
    QString iid = pluginData["IID"].toString();
    QJsonObject metaData = pluginData["MetaData"].toObject();
    
    if (metaData["name"].isUndefined()) {
      qWarning("Missing \"name\" attribute in %s", qPrintable(loader.fileName()));
      continue;
    }
    
    QString name = metaData["name"].toString();
    __availablePlugins.insert(iid, name);
    __files.insert(name, loader.fileName());
    __metadata.insert(name, metaData);
  }
  
  __initialied = true;
}
