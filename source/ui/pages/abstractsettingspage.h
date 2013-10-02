/*
    abstractsettingspage.h
    Copyright (C) 2013  Michał Garapich michal@garapich.pl

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


#ifndef ABSTRACTSETTINGSPAGE_H
#define ABSTRACTSETTINGSPAGE_H

#include <QString>
#include <QWidget>
#include <QVariant>

#include "storage/settingsmanager.h"

class QSettings;

class AbstractSettingsPage : public QWidget {
  
  /*
   * Subclass this class to create a single page in SettingsWindow. 
   */
  
  Q_OBJECT
  
public:
  
  AbstractSettingsPage(QWidget* = 0);
  
  /**
   * Name of the page that will appear in the list.
   */
  virtual QString listElement() const = 0;
  
  /**
   * Icon path that will appear in the list.
   */
  virtual QString listIcon() const = 0;
  
  /**
   * The page's name that will appear in SettingsManager's registry,
   * before the dot.
   */
  virtual QString pageName() const = 0;
  
  /**
   * Puts all values that the page handles in the settingsmanager's
   * map, using setValue().
   */
  virtual void updateFromUi() const = 0;
  
  void restoreSettings(QSettings&);
  void saveSettings(QSettings&);
  
protected:
  void setValue(const QString&, QVariant&&) const;
  
  virtual void __restore(QSettings&) = 0;
  virtual void __save(QSettings&) = 0;
  
};

// shortcut for setValue()
#define SV(key, value) setValue

#define DECLARE_SETTINGS_PAGE(name)

#endif // ABSTRACTSETTINGSPAGE_H
