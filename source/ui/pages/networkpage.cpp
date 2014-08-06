/*
 * networkpage.cpp
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
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

#include "plugins/weatherforecastinterface.h"
#include "storage/pluginmanager.h"
#include "vatsinatorapplication.h"

#include "networkpage.h"

/* Default settings for NetworkPage */
namespace DefaultSettings {
  static const bool    AUTO_UPDATER             = true;
  static const int     REFRESH_RATE             = 3;
  static const bool    METARS_REFRESH           = true;
  static const bool    CACHE_ENABLED            = true;
  static const bool    DATABASE_INTEGRATION     = true;
  static const QString WEATHER_FORECASTS_PROVIDER = "";
  static const QString WEATHER_TEMPERATURE_UNITS  = "Celsius";
}

NetworkPage::NetworkPage(QWidget* _parent) :
    QWidget(_parent) {
  setupUi(this);
  connect(RefreshRateBox,       SIGNAL(valueChanged(int)),
          this,                 SLOT(__updateRefreshRateLabel(int)));
  connect(AutoUpdaterCheckBox,  SIGNAL(stateChanged(int)),
          this,                 SLOT(__updateAutoUpdaterLocks(int)));
}

QString
NetworkPage::listElement() const {
  return tr("Network");
}

QString
NetworkPage::listIcon() const {
  return ":/settings/preferences-network.png";
}

QString
NetworkPage::pageName() const {
  return "network";
}

void
NetworkPage::updateFromUi() const {
  setValue("auto_updater", AutoUpdaterCheckBox->isChecked());
  setValue("refresh_rate", RefreshRateBox->value());
  setValue("cache_enabled", CachingCheckBox->isChecked());
  setValue("refresh_metars", RefreshMetarsCheckBox->isChecked());
  setValue("database_integration", DatabaseIntegrationCheckBox->isChecked());
  setValue("weather_forecast_provider", WeatherProviderListWidget->currentItem()->text());
  setValue("weather_temperature_units", CelsiusRadioButton->isChecked() ? QString("Celsius") : QString("Fahrenheit"));
}

void
NetworkPage::restore(QSettings& _s) {
  bool state = _s.value("auto_updater", DefaultSettings::AUTO_UPDATER).toBool();
  AutoUpdaterCheckBox->setChecked(state);
  __updateAutoUpdaterLocks(state ? Qt::Checked : Qt::Unchecked);
  
  int val = _s.value("refresh_rate", DefaultSettings::REFRESH_RATE).toInt();
  RefreshRateBox->setValue(val);
  __updateRefreshRateLabel(val);
  
  RefreshMetarsCheckBox->setChecked(
    _s.value("refresh_metars", DefaultSettings::METARS_REFRESH).toBool());
  CachingCheckBox->setChecked(
    _s.value("cache_enabled", DefaultSettings::CACHE_ENABLED).toBool());
  DatabaseIntegrationCheckBox->setChecked(
    _s.value("database_integration", DefaultSettings::DATABASE_INTEGRATION).toBool());
  
  QList<WeatherForecastInterface*> weatherPlugins =
    VatsinatorApplication::getSingleton().plugins()->plugins<WeatherForecastInterface>();
  QString weatherCurrent =
    _s.value("weather_forecast_provider", DefaultSettings::WEATHER_FORECASTS_PROVIDER).toString();
  int i = 1;
  for (WeatherForecastInterface* w: weatherPlugins) {
    WeatherProviderListWidget->addItem(w->providerName());
    if (weatherCurrent == w->providerName())
      WeatherProviderListWidget->setCurrentRow(i);
    
    i += 1;
  }
  
  QString units;
  if (!_s.contains("weather_temperature_units")) {
    /* In USA provide Fahrenheit by default */
    if (QLocale::system().country() == QLocale::UnitedStates) {
      units = "Fahrenheit";
    } else {
      units = DefaultSettings::WEATHER_TEMPERATURE_UNITS;
    }
  } else {
    units = _s.value("weather_temperature_units", DefaultSettings::WEATHER_TEMPERATURE_UNITS).toString();
  }
  
  if (units != "Celsius" && units != "Fahrenheit")
    units = DefaultSettings::WEATHER_TEMPERATURE_UNITS;
  
  if (units == "Celsius")
    CelsiusRadioButton->setChecked(true);
  else
    FahrenheitRadioButton->setChecked(true);
}

void
NetworkPage::save(QSettings& _s) {
  _s.setValue("auto_updater", AutoUpdaterCheckBox->isChecked());
  _s.setValue("refresh_rate", RefreshRateBox->value());
  _s.setValue("refresh_metars", RefreshMetarsCheckBox->isChecked());
  _s.setValue("cache_enabled", CachingCheckBox->isChecked());
  _s.setValue("database_integration", DatabaseIntegrationCheckBox->isChecked());
  _s.setValue("weather_forecast_provider", WeatherProviderListWidget->currentItem()->text());
  _s.setValue("weather_temperature_units", CelsiusRadioButton->isChecked() ? QString("Celsius") : QString("Fahrenheit"));
}

void
NetworkPage::__updateRefreshRateLabel(int _n) {
  RefreshRateLabel->setText(tr("minute(s)", "", _n));
}

void
NetworkPage::__updateAutoUpdaterLocks(int _state) {
  CustomUpdatesBox->setEnabled(_state == Qt::Checked ? false : true);
}
