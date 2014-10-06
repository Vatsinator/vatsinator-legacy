/*
    abstractsettingsmodule.h
    Copyright (C) 2013-2014  Michał Garapich michal@garapich.pl

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


#ifndef ABSTRACTSETTINGSMODULE_H
#define ABSTRACTSETTINGSMODULE_H

#include <QString>
#include <QVariant>

#include "storage/settingsmanager.h"

class QSettings;


class AbstractSettingsModule {
  
public:
  
  AbstractSettingsModule();
  
  /**
   * Name of the module.
   */
  virtual QString listElement() const = 0;
  
  /**
   * Icon path to identity the module.
   */
  virtual QString listIcon() const = 0;
  
  /**
   * The module's id that will appear in SettingsManager's registry,
   * before the dot.
   */
  virtual QString moduleId() const = 0;
  
  /**
   * Puts all values that the module handles in the settingsmanager's
   * map, using setValue().
   */
  virtual void update() const = 0;
  
  void restoreSettings(QSettings& settings);
  void saveSettings(QSettings& settings);
  
protected:
  void setValue(const QString& key, QVariant&& value) const;
  
  /**
   * Restores user's settings.
   */
  virtual void restore(QSettings& settings) = 0;
  
  /**
   * Saves user's settings.
   */
  virtual void save(QSettings& settings) = 0;
  
};

#endif // ABSTRACTSETTINGSMODULE_H
