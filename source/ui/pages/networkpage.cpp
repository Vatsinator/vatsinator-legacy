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

#include "plugins/weatherforecastinterface.h"
#include "storage/filemanager.h"
#include "ui/pages/pluginlistwidgetitem.h"
#include "vatsinatorapplication.h"

#include "networkpage.h"

NetworkPage::NetworkPage(QWidget* parent) :
    QWidget(parent) {
  setupUi(this);
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
  setValue("database_integration", DatabaseIntegrationCheckBox->isChecked());
  setValue("weather_forecast_provider", WeatherProviderListWidget->currentItem()->data(Qt::UserRole));
  setValue("weather_temperature_units", CelsiusRadioButton->isChecked() ? QString("Celsius") : QString("Fahrenheit"));
}

void
NetworkPage::restore(QSettings& s, const QVariantHash& defaults) {
  QString id = moduleId() % ".";
  
  DatabaseIntegrationCheckBox->setChecked(
    s.value("database_integration", defaults[id % "database_integration"]).toBool());
  QString weatherCurrent =
    s.value("weather_forecast_provider", defaults[id % "weather_forecast_provider"]).toString();
  
  PluginListWidgetItem* none = new PluginListWidgetItem(tr("None"), WeatherProviderListWidget);
  none->setData(Qt::UserRole, QString("none"));
  WeatherProviderListWidget->setCurrentItem(none);
  
  /* TODO Move this part below to PluginManager */
  QDir pluginsDir(FileManager::staticPath(FileManager::Plugins));
  for (QString fileName: pluginsDir.entryList(QDir::Files)) {
    QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
    QJsonObject pluginData = loader.metaData();
    if (pluginData["IID"].toString() == "org.eu.vatsinator.Vatsinator.WeatherForecastInterface") {
      QJsonObject metaData = pluginData["MetaData"].toObject();
      QString name = metaData["name"].toString();
      QString providerName = metaData["provider_name"].toString();
      
      PluginListWidgetItem* item = new PluginListWidgetItem(providerName, WeatherProviderListWidget);
      item->setData(Qt::UserRole, name);
      
      if (weatherCurrent == name)
        WeatherProviderListWidget->setCurrentItem(item);
    }
  }
  
  QString units = s.value("weather_temperature_units", defaults[id % "weather_temperature_units"]).toString();
  
  if (units != "Celsius" && units != "Fahrenheit")
    units = defaults[id % "weather_temperature_units"].toString();
  
  if (units == "Celsius")
    CelsiusRadioButton->setChecked(true);
  else
    FahrenheitRadioButton->setChecked(true);
}

void
NetworkPage::save(QSettings& s) {
  s.setValue("database_integration", DatabaseIntegrationCheckBox->isChecked());
  s.setValue("weather_forecast_provider", WeatherProviderListWidget->currentItem()->data(Qt::UserRole));
  s.setValue("weather_temperature_units", CelsiusRadioButton->isChecked() ? QString("Celsius") : QString("Fahrenheit"));
}
