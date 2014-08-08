/*
    settingsmanager.h
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QColor>
#include <QVariant>

#include "singleton.h"

class AbstractSettingsModule;

/**
 * This class gives us access to global user configuration.
 * Usage is pretty rich, so avoid it if unnesessary or store
 * settings in local variables. You can use settingsChanged()
 * signal to update needed settings.
 * It works like this:
 * SM::get("network.refresh_rate").toInt();
 * SM is typedef for SettingsManager.
 */
class SettingsManager : public QObject {
  Q_OBJECT
  
  friend class AbstractSettingsModule;

signals:
  void settingsChanged();

public:
  SettingsManager(QObject* = 0);
  
  /**
   * Adds settings page.
   */
  void addPage(AbstractSettingsModule*);
  
  /**
   * We need this to be accessible before all pages are created.
   * All pages are QWidget-dependent, so they can not be constructed
   * before whole UI.
   */
  static QString earlyGetLocale();
  
  /**
   * Gives access to particular setting.
   * @param s Page and variable name, glued by dot (.).
   *    Example: SettingsManager::get("network.refresh_rate")
   */
  static const QVariant& get(const QString&);
  
  /**
   * If any class modifies the QSettings directly, it should call
   * this method in order to have changes visible in the UI.
   * @param pName Page name.
   */
  static void updateUi(const QString&);
  
public slots:
  void initialize();
  
  /**
   * Saves all settings to local config file.
   */
  void saveSettings();
  
  /**
   * Clears all entries and then calls __restoreSettings().
   */
  void restoreDefaults();

private:
  
  /**
   * Sets the given value in the settings map.
   * @param key Key of the value.
   * @param value The value.
   */
  static void updateValue(QString&&, QVariant&&);
  
  /**
   * Reads config file, restores all settings.
   */
  void __restoreSettings();
  
  /**
   * Gets page by module id.
   */
  AbstractSettingsModule* __getPage(const QString&) const;
  
  /**
   * Fills the default settings map.
   */
  void __fillDefaults();
  
  
  QList<AbstractSettingsModule*> __pages;
  QVariantHash __settings;
  QVariantHash __defaults;

};

typedef SettingsManager SM;

#endif // SETTINGSMANAGER_H
