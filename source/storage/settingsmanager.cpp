/*
    settingsmanager.cpp
    Copyright (C) 2012-2015  Michał Garapich michal@garapich.pl

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

#include <QtCore>

#include "storage/abstractsettingsmodule.h"
#include "ui/userinterface.h"
#include "vatsinatorapplication.h"

#include "settingsmanager.h"

SettingsManager::SettingsManager(QObject* parent) :
    QObject(parent)
{
    connect(vApp()->userInterface(),      SIGNAL(initialized()),
            this,                         SLOT(initialize()));
    __fillDefaults();
    __settings = __defaults;
}

void
SettingsManager::addPage(AbstractSettingsModule* page)
{
    __pages << page;
}

QString
SettingsManager::earlyGetLocale()
{
    QSettings settings;
    settings.beginGroup("Settings/misc");
    
    QString language = settings.value("language",
                                      QLocale::system().name()).toString();
                                      
    settings.endGroup();
    return language;
}

const QVariant&
SettingsManager::get(const QString& s)
{
    Q_ASSERT_X(vApp()->settingsManager()->__settings.contains(s),
               qPrintable(QString("SettingsManager::get(%1)").arg(s)),
               "No such value");
               
    return vApp()->settingsManager()->__settings[s];
}

void
SettingsManager::updateUi(const QString& pageName)
{
    AbstractSettingsModule *m = vApp()->settingsManager()->__getPage(pageName);
    
#ifdef Q_OS_ANDROID
    /* TODO Remove when Android settings are done */
    if (!m)
        return;
#endif
    
    Q_ASSERT(m);
               
    QSettings s;
    s.beginGroup("Settings");
    m->restoreSettings(s, vApp()->settingsManager()->__defaults);
    s.endGroup();
}

void
SettingsManager::initialize()
{
    __restoreSettings();
}

void
SettingsManager::updateSettings()
{
    for (AbstractSettingsModule* p : __pages)
        p->update();
        
    emit settingsChanged();
}

void
SettingsManager::saveSettings()
{
    QSettings s;
    s.beginGroup("Settings");
    
    for (AbstractSettingsModule* p : __pages) {
        p->saveSettings(s);
        p->update();
    }
    
    s.endGroup();
    
    emit settingsChanged();
}

void
SettingsManager::restoreDefaults()
{
    QSettings s;
    s.beginGroup("Settings");
    
    s.remove("");
    
    for (AbstractSettingsModule* p : __pages)
        p->restoreSettings(s, __defaults);
        
    s.endGroup();
}

void
SettingsManager::updateValue(QString&& key, QVariant&& value)
{
    vApp()->settingsManager()->__settings[key] = value;
}

void
SettingsManager::__restoreSettings()
{
    QSettings s;
    s.beginGroup("Settings");
    
    for (AbstractSettingsModule* p : __pages) {
        p->restoreSettings(s, __defaults);
        p->update();
    }
    
    s.endGroup();
    
    emit settingsChanged();
}

AbstractSettingsModule*
SettingsManager::__getPage(const QString& s) const
{
    for (AbstractSettingsModule* p : __pages)
        if (p->moduleId() == s)
            return p;
            
    return nullptr;
}

void
SettingsManager::__fillDefaults()
{
    __defaults["network.database_integration"] = true;
    
    /* In USA provide Fahrenheit by default */
    if (QLocale::system().country() == QLocale::UnitedStates)
        __defaults["network.weather_temperature_units"] = "Fahrenheit";
    else
        __defaults["network.weather_temperature_units"] = "Celsius";
        
    __defaults["misc.send_statistics"] = true;
    __defaults["misc.language"] = QLocale::system().name().left(2);
    
    __defaults["view.pilots_layer"] = true;
    __defaults["view.airports_layer"] = true;
    __defaults["view.staffed_firs"] = true;
    __defaults["view.unstaffed_firs"] = true;
    __defaults["view.empty_airports"] = false;
    __defaults["view.pilot_labels.always"] = true;
    __defaults["view.pilot_labels.when_hovered"] = true;
    __defaults["view.pilot_labels.airport_related"] = true;
    __defaults["view.airport_labels"] = true;
}
