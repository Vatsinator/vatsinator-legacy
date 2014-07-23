/*
 * pluginmanager.h
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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QList>

class PluginManager : public QObject {
  Q_OBJECT
  
  /**
   * The PluginManager class handles all Vatsinator plugins.
   */

public:
  explicit PluginManager(QObject* = 0);
  
  /**
   * Loads all plugins from the plugin directory.
   */
  void loadPlugins();
  
  /**
   * Gives access to all loaded plugins that implement the interface
   * specified by T.
   */
  template <typename T>
    QList<T*> plugins() const {
      QList<T*> p;
      for (QObject* o: __plugins) {
        if (T* q = qobject_cast<T*>(o))
          p << q;
      }
      
      return std::move(p);
    }
  
private:
  
  /**
   * List of loaded plugins.
   */
  QList<QObject*> __plugins;
  
};

#endif // PLUGINMANAGER_H
