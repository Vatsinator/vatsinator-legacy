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
#include "ui/widgetsuserinterface.h"
#include "ui/vatsinatorstyle.h"
#include "vatsinatorapplication.h"

#include "miscellaneouspage.h"

MiscellaneousPage::MiscellaneousPage(QWidget* parent) : QWidget(parent) {
  setupUi(this);
  VatsinatorStyle* style = qobject_cast<VatsinatorStyle*>(vApp()->style());
  StatsDescriptionLabel->setFont(style->smallFont());
  LanguageDescriptionLabel->setFont(style->smallFont());
  PrivacyPolicyLabel->setFont(style->smallFont());
  
  LanguageComboBox->addItems(vApp()->languageManager()->allLanguages());

  connect(this, &MiscellaneousPage::languageChanged,
          wui(), &WidgetsUserInterface::showAppRestartDialog);

  connect(StatsCheckBox, &QCheckBox::stateChanged, this, &MiscellaneousPage::settingsChanged);
}

QString
MiscellaneousPage::listElement() const {
  return tr("Miscellaneous");
}

QString
MiscellaneousPage::listIcon() const {
  return QStringLiteral(":/settings/preferences-miscellaneous.png");
}

QString
MiscellaneousPage::moduleId() const {
  return QStringLiteral("misc");
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
MiscellaneousPage::restore(QSettings& s, const QVariantHash& defaults) {
  QString id = moduleId() % ".";
  StatsCheckBox->setChecked(
    s.value("send_statistics", defaults[id % "send_statistics"]).toBool());
  LanguageComboBox->setCurrentIndex(
    vApp()->languageManager()->getLanguageId(
      s.value("language", defaults[id % "language"]).toString()
    )
  );
}

void
MiscellaneousPage::save(QSettings& s) {
  s.setValue("send_statistics", StatsCheckBox->isChecked());
  s.setValue("language", vApp()->languageManager()->getLocaleById(LanguageComboBox->currentIndex()));
  
  if (__languageIndex != LanguageComboBox->currentIndex())
    emit languageChanged();
}
