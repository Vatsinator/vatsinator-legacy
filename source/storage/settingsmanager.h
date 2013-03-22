/*
    settingsmanager.h
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl

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
#include "ui/pages/pagelist.h"

class AbstractSettingsPage;
class SettingsWindow;

class SettingsManager :
    public QObject,
    public Singleton< SettingsManager > {
  
  /*
   * This class gives us access to global user configuration.
   * Usage is pretty rich, so avoid it if unnesessary or store
   * settings in local variables. You can use settingsChanged()
   * signal to update needed settings.
   * It works like this:
   * SM::get("misc.refresh_rate").toInt();
   * SM is typedef for SettingsManager.
   */

  Q_OBJECT

public:
  SettingsManager(QObject* = 0);
  virtual ~SettingsManager();
  
  void init();
  
  /**
   * We need this to be accessible before all pages are created.
   * All pages are QWidget-dependent, so they can not be constructed
   * before whole UI.
   */
  static QString earlyGetLocale();
  
  /**
   * Gives access to particular setting.
   * @param s Page and variable name, glued by dot (.).
   *    Example: SettingsManager::get("misc.refresh_rate")
   */
  static QVariant get(const QString&);

private:
  /**
   * Reads config file, restores all settings.
   */
  void __restoreSettings();
  
  /**
   * Saves all settings to local config file.
   */
  void __saveSettings();
  
  const AbstractSettingsPage* __parsePage(const QString&) const;
  
  AbstractSettingsPage* __pages[PageList::__count];
  
signals:
  void settingsChanged();

};

typedef SettingsManager SM;

#endif // SETTINGSMANAGER_H
