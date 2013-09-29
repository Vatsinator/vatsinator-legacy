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
   * Setting accessed via QString.
   */
  virtual QVariant get(const QString&) const = 0;
  
  void restoreSettings(QSettings&);
  void saveSettings(QSettings&);
  
protected:
  virtual void __restore(QSettings&) = 0;
  virtual void __save(QSettings&) = 0;
  
public:
  /**
   * Needed and used by SettingsManager.
   */
  const QString __sm_page_name_s;
  
  virtual const QString &
  __sm_page_name() const { return __sm_page_name_s; }
};

#define VATSINATOR_DECLARE_PAGE(x)                          \
  private:                                                  \
    const QString __sm_page_name_s = QString(#x).toLower(); \
  public:                                                   \
    const QString& __sm_page_name() const {                 \
      return __sm_page_name_s;                              \
    }                                                       \
  private:

#define _S(name, value) if (_s == #name) return value

#define _S_END                                             \
  Q_ASSERT_X(false, qPrintable(__sm_page_name()), "No such variable"); \
  return QVariant()

#endif // ABSTRACTSETTINGSPAGE_H
