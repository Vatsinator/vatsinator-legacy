/*
 * mappage.cpp
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#include "plugins/mapdrawer.h"
#include "ui/map/maprenderer.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/widgetsuserinterface.h"

#include "mappage.h"

MapPage::MapPage(QWidget* parent) : WidgetSettingsModule(parent)
{
    setupUi(this);
    
    connect(FirFontSelectionButton, &QPushButton::clicked, this, &MapPage::__showFirFontDialog);
    connect(AirportFontSelectionButton, &QPushButton::clicked, this, &MapPage::__showAirportFontDialog);
    connect(PilotFontSelectionButton, &QPushButton::clicked, this, &MapPage::__showPilotFontDialog);
    
    connect(MapTypeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &MapPage::settingsChanged);
    
    MapTypeComboBox->setProperty("vatsinatorSettingsKey", "map_type");
    
    connect(wui(), &WidgetsUserInterface::initialized, this, &MapPage::__initialize);
}

QString
MapPage::listElement() const
{
    return tr("Map");
}

QString
MapPage::listIcon() const
{
    return QStringLiteral(":/settings/preferences-colors.png");
}

QString
MapPage::moduleId() const
{
    return QStringLiteral("map");
}

void
MapPage::update() const
{
    WidgetSettingsModule::update();
    
    setValue("fir_font", __firFont);
    setValue("airport_font", __airportFont);
    setValue("pilot_font", __pilotFont);
}

void
MapPage::restore(QSettings& s, const QVariantHash& defaults)
{
    WidgetSettingsModule::restore(s, defaults);
    
    QString id = moduleId() % ".";
    
    __firFont = s.value("fir_font", defaults[id % "fir_font"]).value<QFont>();
    __airportFont = s.value("airport_font", defaults[id % "airport_font"]).value<QFont>();
    __pilotFont = s.value("pilot_font", defaults[id % "pilot_font"]).value<QFont>();
    
    __updateFontButtons();
}

void
MapPage::save(QSettings& s)
{
    WidgetSettingsModule::save(s);
    
    s.setValue("fir_font", __firFont);
    s.setValue("airport_font", __airportFont);
    s.setValue("pilot_font", __pilotFont);
}

void
MapPage::__initialize()
{
    MapRenderer* renderer = wui()->mainWindow()->mapWidget()->renderer();
    connect(renderer, &MapRenderer::mapDrawerChanged, this, &MapPage::__updateMapTypes);
    
    __updateMapTypes(renderer->mapDrawer());
}

void
MapPage::__updateFontButtons()
{
    QString fontName = QString("%1, %2").arg(__firFont.family(), QString::number(__firFont.pointSize()));
    FirFontSelectionButton->setText(fontName);
    FirFontSelectionButton->setFont(__firFont);
    
    fontName = QString("%1, %2").arg(__airportFont.family(), QString::number(__airportFont.pointSize()));
    AirportFontSelectionButton->setText(fontName);
    AirportFontSelectionButton->setFont(__airportFont);
    
    fontName = QString("%1, %2").arg(__pilotFont.family(), QString::number(__pilotFont.pointSize()));
    PilotFontSelectionButton->setText(fontName);
    PilotFontSelectionButton->setFont(__pilotFont);
}

void
MapPage::__showFirFontDialog()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, __firFont, this);
    
    if (ok) {
        __firFont = font;
        __updateFontButtons();
        emit settingsChanged();
    }
}

void
MapPage::__showAirportFontDialog()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, __airportFont, this);
    
    if (ok) {
        __airportFont = font;
        __updateFontButtons();
        emit settingsChanged();
    }
}

void
MapPage::__showPilotFontDialog()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, __pilotFont, this);
    
    if (ok) {
        __pilotFont = font;
        __updateFontButtons();
        emit settingsChanged();
    }
}

void
MapPage::__updateMapTypes(MapDrawer* drawer)
{
    MapTypeComboBox->clear();
    
    if (!drawer) {
        MapTypeComboBox->setEnabled(false);
    } else {
        auto types = drawer->types();
        std::for_each(types.begin(), types.end(), [this](const QString& type) {
            MapTypeComboBox->addItem(type);
        });
    }
}
