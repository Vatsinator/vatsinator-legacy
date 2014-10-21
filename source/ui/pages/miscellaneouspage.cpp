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

#include <QtWidgets>

#include "storage/languagemanager.h"
#include "ui/userinterface.h"
#include "vatsinatorapplication.h"

#include "miscellaneouspage.h"

/* Default settings for MiscellaneousPage */
namespace DefaultSettings {
  static const bool    SEND_STATS       = true;
  static const QString LANGUAGE         = QLocale::system().name().left(2);
}

MiscellaneousPage::MiscellaneousPage(QWidget* parent) :
    QWidget(parent) {
  setupUi(this);
  LanguageComboBox->addItems(LanguageManager::getSingleton().allLanguages());
  
  connect(this,                         SIGNAL(languageChanged()),
          vApp()->userInterface(),      SLOT(showAppRestartDialog()));
}

QString
MiscellaneousPage::listElement() const {
  return tr("Miscellaneous");
}

QString
MiscellaneousPage::listIcon() const {
  return ":/settings/preferences-miscellaneous.png";
}

QString
MiscellaneousPage::moduleId() const {
  return "misc";
}

void
MiscellaneousPage::update() const {
  setValue("send_statistics",
           StatsCheckBox->isChecked());
  /* TODO fix LanguageManager instance below */
//   setValue("language",
//            LanguageManager::getSingleton().getLocaleById(LanguageComboBox->currentIndex()));
}

void
MiscellaneousPage::showEvent(QShowEvent* event) {
  Q_UNUSED(event);
  __languageIndex = LanguageComboBox->currentIndex();
}

void
MiscellaneousPage::restore(QSettings& s) {
  StatsCheckBox->setChecked(
    s.value("send_statistics", DefaultSettings::SEND_STATS).toBool());
  LanguageComboBox->setCurrentIndex(
    LanguageManager::getSingleton().getLanguageId(
      s.value("language", DefaultSettings::LANGUAGE).toString()
    )
  );
}

void
MiscellaneousPage::save(QSettings& s) {
  s.setValue("send_statistics", StatsCheckBox->isChecked());
  s.setValue("language", LanguageManager::getSingleton().getLocaleById(LanguageComboBox->currentIndex()));
  
  if (__languageIndex != LanguageComboBox->currentIndex())
    emit languageChanged();
}
