/*
    abstractsettingspage.cpp
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

#include <QtGui>

#include "abstractsettingspage.h"
#include "defines.h"

AbstractSettingsPage::AbstractSettingsPage(QWidget* _parent) :
  QWidget(_parent),
  __sm_page_name("") {}

void
AbstractSettingsPage::restoreSettings(QSettings& _s) {
  _s.beginGroup(__sm_page_name());
  __restore(_s);
  _s.endGroup();
}

void
AbstractSettingsPage::saveSettings(QSettings& _s) {
  _s.beginGroup(__sm_page_name());
  __save(_s);
  _s.endGroup();
}
