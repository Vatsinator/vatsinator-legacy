/*
 * networkpage.cpp
 * Copyright (C) 2013-2015  Michał Garapich <michal@garapich.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QtWidgets>

#include "ui/vatsinatorstyle.h"
#include "vatsinatorapplication.h"

#include "networkpage.h"

NetworkPage::NetworkPage(QWidget* parent) : QWidget(parent) {
  setupUi(this);
  
  VatsinatorStyle* style = qobject_cast<VatsinatorStyle*>(vApp()->style());
  VatsinatorDatabaseDescriptionLabel->setFont(style->smallFont());
  
  connect(DatabaseUpdatesCheckBox, &QCheckBox::stateChanged, this, &NetworkPage::settingsChanged);
  connect(TemperatureUnitsComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &NetworkPage::settingsChanged);
}

QString
NetworkPage::listElement() const {
  return tr("Network");
}

QString
NetworkPage::listIcon() const {
  return QStringLiteral(":/settings/preferences-network.png");
}

QString
NetworkPage::moduleId() const {
  return QStringLiteral("network");
}

void
NetworkPage::update() const {
  setValue("database_integration", DatabaseUpdatesCheckBox->isChecked());
  setValue("weather_temperature_units", TemperatureUnitsComboBox->currentIndex() == 0 ? QStringLiteral("Celsius") : QStringLiteral("Fahrenheit"));
}

void
NetworkPage::restore(QSettings& s, const QVariantHash& defaults) {
  QString id = moduleId() % ".";
  
  DatabaseUpdatesCheckBox->setChecked(
    s.value("database_integration", defaults[id % "database_integration"]).toBool());
  
  QString units = s.value("weather_temperature_units", defaults[id % "weather_temperature_units"]).toString();
  if (units != "Celsius" && units != "Fahrenheit")
    units = defaults[id % "weather_temperature_units"].toString();

  if (units == QStringLiteral("Celsius"))
    TemperatureUnitsComboBox->setCurrentIndex(0);
  else
    TemperatureUnitsComboBox->setCurrentIndex(1);
}

void
NetworkPage::save(QSettings& s) {
  s.setValue("database_integration", DatabaseUpdatesCheckBox->isChecked());
  s.setValue("weather_temperature_units", TemperatureUnitsComboBox->currentIndex() == 0 ? QStringLiteral("Celsius") : QStringLiteral("Fahrenheit"));
}
