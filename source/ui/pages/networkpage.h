/*
 * networkpage.h
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

#ifndef NETWORKPAGE_H
#define NETWORKPAGE_H

#include "ui/ui_networkpage.h"
#include "storage/abstractsettingsmodule.h"

class NetworkPage : public QWidget, public AbstractSettingsModule, private Ui::NetworkPage {
    Q_OBJECT
    
signals:
    void settingsChanged();
    
public:
    NetworkPage(QWidget* parent = nullptr);
    
    QString listElement() const override;
    QString listIcon() const override;
    QString moduleId() const override;
    
    /**
     * @variables
     * database_integration:              bool
     * weather_forecast_provider:         string
     * weather_temperature_units:         string {"Celsius", "Fahrenheit"}
     */
    void update() const;
    
protected:
    void restore(QSettings& s, const QVariantHash& defaults) override;
    void save(QSettings& s) override;
    
};

#endif // NETWORKPAGE_H
