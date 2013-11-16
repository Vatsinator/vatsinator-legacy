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

#include <QtGui>

#include "networkpage.h"
#include "defines.h"

/* Default settings for NetworkPage */
namespace DefaultSettings {
  static const bool    AUTO_UPDATER      = true;
  static const int     REFRESH_RATE      = 3;
  static const bool    METARS_REFRESH    = true;
  static const bool    CACHE_ENABLED     = true;
  static const bool    VERSION_CHECK     = true;
  static const bool    WEATHER_FORECASTS = true;
}

NetworkPage::NetworkPage(QWidget* _parent) :
    AbstractSettingsPage(_parent) {
  setupUi(this);
  connect(RefreshRateBox, SIGNAL(valueChanged(int)),
          this,           SLOT(__updateRefreshRateLabel(int)));
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
  setValue("version_check", VersionCheckingCheckBox->isChecked());
  setValue("weather_forecasts", WeatherForecastCheckBox->isChecked());
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
  VersionCheckingCheckBox->setChecked(
    _s.value("version_check", DefaultSettings::VERSION_CHECK).toBool());
  WeatherForecastCheckBox->setChecked(
    _s.value("weather_forecasts", DefaultSettings::WEATHER_FORECASTS).toBool());
}

void
NetworkPage::save(QSettings& _s) {
  _s.setValue("auto_updater", AutoUpdaterCheckBox->isChecked());
  _s.setValue("refresh_rate", RefreshRateBox->value());
  _s.setValue("refresh_metars", RefreshMetarsCheckBox->isChecked());
  _s.setValue("cache_enabled", CachingCheckBox->isChecked());
  _s.setValue("version_check", VersionCheckingCheckBox->isChecked());
  _s.setValue("weather_forecasts", WeatherForecastCheckBox->isChecked());
}

void
NetworkPage::__updateRefreshRateLabel(int _n) {
  RefreshRateLabel->setText(tr("minute(s)", "", _n));
}

void
NetworkPage::__updateAutoUpdaterLocks(int _state) {
  CustomUpdatesBox->setEnabled(_state == Qt::Checked ? false : true);
}
