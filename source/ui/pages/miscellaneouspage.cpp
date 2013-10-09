/*
    miscellaneouspage.cpp
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

#include "storage/languagemanager.h"

#include "miscellaneouspage.h"
#include "defines.h"

/* Default settings for MiscellaneousPage */
namespace DefaultSettings {
  static const bool    ANTYALIASING     = false;
  static const int     ZOOM_COEFFICIENT = 30;
  static const bool    SEND_STATS       = true;
  static const QString LANGUAGE         = QLocale::system().name().left(2);
}

MiscellaneousPage::MiscellaneousPage(QWidget* _parent) :
    AbstractSettingsPage(_parent) {
  setupUi(this);
  LanguageComboBox->addItems(LanguageManager::getSingleton().allLanguages());
}

QString
MiscellaneousPage::listElement() const {
  return tr("Miscellaneous");
}

QString
MiscellaneousPage::listIcon() const {
  return ":/settings/preferences-miscellaneous.png";
}

QVariant
MiscellaneousPage::get(const QString& _s) const {
  _S(has_antyaliasing,  AntyaliasingCheckBox->isChecked());
  _S(zoom_coefficient,  ZoomCoefficientSlider->value());
  _S(send_statistics,   StatsCheckBox->isChecked());
  _S(language,          LanguageManager::getSingleton().getLocaleById(LanguageComboBox->currentIndex()));
  
  _S_END;
}

void
MiscellaneousPage::__restore(QSettings& _s) {
  AntyaliasingCheckBox->setChecked(
    _s.value("has_antyaliasing", DefaultSettings::ANTYALIASING).toBool());
  ZoomCoefficientSlider->setValue(
    _s.value("zoom_coefficient", DefaultSettings::ZOOM_COEFFICIENT).toInt());
  StatsCheckBox->setChecked(
    _s.value("send_statistics", DefaultSettings::SEND_STATS).toBool());
  LanguageComboBox->setCurrentIndex(
    LanguageManager::getSingleton().getLanguageId(
      _s.value("language", DefaultSettings::LANGUAGE).toString()
    )
  );
}

void
MiscellaneousPage::__save(QSettings& _s) {
  _s.setValue("has_antyaliasing", AntyaliasingCheckBox->isChecked());
  _s.setValue("zoom_coefficient", ZoomCoefficientSlider->value());
  _s.setValue("send_statistics", StatsCheckBox->isChecked());
  _s.setValue("language", LanguageManager::getSingleton().getLocaleById(LanguageComboBox->currentIndex()));
}