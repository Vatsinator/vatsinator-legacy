/*
    abstractsettingspage.cpp
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

#include <QtWidgets>

#include "storage/settingsmanager.h"

#include "abstractsettingspage.h"

AbstractSettingsPage::AbstractSettingsPage() {}

void
AbstractSettingsPage::restoreSettings(QSettings& _s) {
  _s.beginGroup(pageName());
  restore(_s);
  _s.endGroup();
}

void
AbstractSettingsPage::saveSettings(QSettings& _s) {
  _s.beginGroup(pageName());
  save(_s);
  _s.endGroup();
}

void
AbstractSettingsPage::setValue(const QString& _key, QVariant&& _value) const {
  SettingsManager::updateValue(pageName() % "." % _key,
                            std::forward<QVariant>(_value));
}
