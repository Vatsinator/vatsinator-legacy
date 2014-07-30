/*
 * pluginmanager.cpp
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

PluginManager::PluginManager(QObject* _parent): QObject(_parent) {}

void
PluginManager::loadPlugins() {
  QDir pluginsDir(FileManager::staticPath(FileManager::Plugins));
  
  for (QString fileName: pluginsDir.entryList(QDir::Files)) {
    QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
    QObject* plugin = loader.instance();
    if (plugin) {
      __plugins << plugin;
    }
  }
}
