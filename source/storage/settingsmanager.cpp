/*
    settingsmanager.cpp
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

#include <QtWidgets>

#include "storage/abstractsettingspage.h"
#include "ui/windows/settingswindow.h"
#include "ui/userinterface.h"
#include "vatsinatorapplication.h"

#include "settingsmanager.h"

SettingsManager::SettingsManager(QObject* _parent) :
    QObject(_parent) {
  connect(vApp()->userInterface(),      SIGNAL(initialized()),
          this,                         SLOT(initialize()));
}

void
SettingsManager::addPage(AbstractSettingsPage* _page) {
  __pages << _page;
}

QString
SettingsManager::earlyGetLocale() {
  QSettings settings;
  settings.beginGroup("Settings/misc");
  
  QString language = settings.value("language",
                              QLocale::system().name()).toString();
  
  settings.endGroup();
  return language;
}

const QVariant &
SettingsManager::get(const QString& _s) {
  Q_ASSERT_X(getSingleton().__settings.contains(_s),
             qPrintable(QString("SettingsManager::get(%1)").arg(_s)),
             "No such value");
  
  return getSingleton().__settings[_s];
}

void
SettingsManager::updateUi(const QString& _pName) {
  Q_ASSERT_X(getSingleton().__getPage(_pName),
             qPrintable(QString("SettingsManager::updateUi(%1)").arg(_pName)),
             "No such page");
  
  QSettings s;
  s.beginGroup("Settings");
  getSingleton().__getPage(_pName)->restoreSettings(s);
  s.endGroup();
}

void
SettingsManager::initialize() {
  __restoreSettings();
}

void
SettingsManager::saveSettings() {
  QSettings s;
  s.beginGroup("Settings");
  
  for (AbstractSettingsPage* p: __pages) {
    p->saveSettings(s);
    p->updateFromUi();
  }
  
  s.endGroup();
  
  emit settingsChanged();
}

void
SettingsManager::restoreDefaults() {
  QSettings s;
  s.beginGroup("Settings");
  
  s.remove("");
  
  for (AbstractSettingsPage* p: __pages)
    p->restoreSettings(s);
  
  s.endGroup();
}

void
SettingsManager::updateValue(QString&& _key, QVariant&& _value) {
  getSingleton().__settings[_key] = _value;
}

void
SettingsManager::__restoreSettings() {
  QSettings s;
  s.beginGroup("Settings");
  
  for (AbstractSettingsPage* p: __pages) {
    p->restoreSettings(s);
    p->updateFromUi();
  }
  
  s.endGroup();
  
  emit settingsChanged();
}

AbstractSettingsPage *
SettingsManager::__getPage(const QString& _s) const {
  for (AbstractSettingsPage* p: __pages)
    if (p->pageName() == _s)
      return p;
  
  return nullptr;
}
