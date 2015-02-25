/*
 * pluginmanager.h
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

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QMap>
#include <QMultiMap>
#include <QJsonObject>
#include <QPluginLoader>

/**
 * The PluginManager class is responsible for finding, handling and loading
 * Vatsinator plugins.
 */
class PluginManager : public QObject {
    Q_OBJECT

public:
  explicit PluginManager(QObject* parent = nullptr);
  
  /**
   * Finds plugins by the IID and returns their names.
   * 
   * \param iid The IID of the plugin (i.e. "org.eu.vatsinator.Vatsinator.WeatherForecastInterface").
   * \return List of names of plugins that match the given _iid_ param. The list may be empty.
   */
  QStringList plugins(const QString& iid) const;
  
  /**
   * Reads the plugin's metadata.
   * 
   * \param name The required plugin name.
   * \return The json object that represents the plugin's metadata. It may be empty.
   */
  QJsonObject metadata(const QString& name) const;
  
  /**
   * Loads the plugin (if necessary).
   * 
   * The instance will be casted to T. If the cast fails, _nullptr_ will be returned.
   * 
   * \param T Interface type.
   * \param name The plugin's name.
   */
  template <typename T>
    T* load(const QString& name);
    
  /**
   * Called by VatsinatorApplication only.
   */
  void initialize();
  
private:
  bool __initialied;
  QMultiMap<QString, QString> __availablePlugins; /**< iid <-> name */
  QMap<QString, QJsonObject> __metadata; /**< name <-> json */
  QMap<QString, QString> __files; /**< name <-> file name */
  QMap<QString, QObject*> __loadedPlugins; /**< name <-> instance */
  
};


template <typename T>
  T* PluginManager::load(const QString& name) {
    if (!__files.contains(name)) {
      qWarning("No such plugin: %s", qPrintable(name));
      return nullptr;
    }
    
    if (!__loadedPlugins.contains(name)) {
      QPluginLoader loader(__files[name]);
      QObject* instance = loader.instance();
      if (!instance) {
        qWarning("Failed to load %s (%s)", qPrintable(loader.fileName()), qPrintable(loader.errorString()));
        return nullptr;
      }
      
      if (!qobject_cast<T*>(instance)) {
        loader.unload();
        return nullptr;
      }
      
      __loadedPlugins[name] = static_cast<QObject*>(instance);
    }
    
    return qobject_cast<T*>(__loadedPlugins[name]);
  }


#endif // PLUGINMANAGER_H
