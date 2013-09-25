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
  static const int     REFRESH_RATE     = 3;
  static const bool    PROMPT_ON_ERROR  = false;
  static const bool    METARS_REFRESH   = true;
  static const bool    CACHE_ENABLED    = true;
  static const bool    VERSION_CHECK    = true;
}

NetworkPage::NetworkPage(QWidget* _parent) :
    AbstractSettingsPage(_parent) {
  setupUi(this);
}

QString
NetworkPage::listElement() const {
  return tr("Network");
}

QString
NetworkPage::listIcon() const {
  return ":/settings/preferences-network.png";
}

QVariant
NetworkPage::get(const QString& _s) const {
  _S(refresh_rate,      RefreshRateBox->value());
  _S(prompt_on_error,   PromptOnErrorCheckBox->isChecked());
  _S(cache_enabled,     CachingCheckBox->isChecked());
  _S(refresh_metars,    RefreshMetarsCheckBox->isChecked());
  _S(version_check,     VersionCheckingCheckBox->isChecked());
  
  _S_END;
}

void
NetworkPage::__restore(QSettings& _s) {
  RefreshRateBox->setValue(
    _s.value("refresh_rate", DefaultSettings::REFRESH_RATE).toInt());
  PromptOnErrorCheckBox->setChecked(
    _s.value("prompt_on_error", DefaultSettings::PROMPT_ON_ERROR).toBool());
  RefreshMetarsCheckBox->setChecked(
    _s.value("refresh_metars", DefaultSettings::METARS_REFRESH).toBool());
  CachingCheckBox->setChecked(
    _s.value("cache_enabled", DefaultSettings::CACHE_ENABLED).toBool());
  VersionCheckingCheckBox->setChecked(
    _s.value("version_check", DefaultSettings::VERSION_CHECK).toBool());
}

void
NetworkPage::__save(QSettings& _s) {
  _s.setValue("refresh_rate", RefreshRateBox->value());
  _s.setValue("prompt_on_error", PromptOnErrorCheckBox->isChecked());
  _s.setValue("refresh_metars", RefreshMetarsCheckBox->isChecked());
  _s.setValue("cache_enabled", CachingCheckBox->isChecked());
  _s.setValue("version_check", VersionCheckingCheckBox->isChecked());
}
