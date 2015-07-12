/*
 * mappage.cpp
 * Copyright (C) 2014-2015  Michał Garapich <michal@garapich.pl>
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

#include "mappage.h"

MapPage::MapPage(QWidget* parent) : WidgetSettingsModule(parent)
{
    setupUi(this);
    
#ifdef Q_OS_MAC
    layout()->setSpacing(0);
#endif
    
    connect(FirFontSelectionButton, &QPushButton::clicked, this, &MapPage::__showFirFontDialog);
    connect(AirportFontSelectionButton, &QPushButton::clicked, this, &MapPage::__showAirportFontDialog);
    connect(PilotFontSelectionButton, &QPushButton::clicked, this, &MapPage::__showPilotFontDialog);
    
    connect(ZoomCoefficientSlider, &QSlider::valueChanged, this, &MapPage::settingsChanged);
    connect(StaffedFirColorButton, &ColorButton::colorChanged, this, &MapPage::settingsChanged);
    connect(UnstaffedFirColorButton, &ColorButton::colorChanged, this, &MapPage::settingsChanged);
    connect(StaffedUirColorButton, &ColorButton::colorChanged, this, &MapPage::settingsChanged);
    connect(ApproachCircleColorButton, &ColorButton::colorChanged, this, &MapPage::settingsChanged);
    connect(OriginToPilotLineColorButton, &ColorButton::colorChanged, this, &MapPage::settingsChanged);
    connect(PilotToDestinationLineColorButton, &ColorButton::colorChanged, this, &MapPage::settingsChanged);
    connect(StaffedFirColorAlphaBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MapPage::settingsChanged);
    connect(StaffedUirColorAlphaBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &MapPage::settingsChanged);
    
    ZoomCoefficientSlider->setProperty("vatsinatorSettingsKey", "zoom_coefficient");
    StaffedFirColorButton->setProperty("vatsinatorSettingsKey", "staffed_fir_borders_color");
    StaffedFirColorAlphaBox->setProperty("vatsinatorSettingsKey", "staffed_fir_background_alpha");
    UnstaffedFirColorButton->setProperty("vatsinatorSettingsKey", "unstaffed_fir_borders_color");
    StaffedUirColorButton->setProperty("vatsinatorSettingsKey", "staffed_uir_borders_color");
    StaffedUirColorAlphaBox->setProperty("vatsinatorSettingsKey", "staffed_uir_background_alpha");
    ApproachCircleColorButton->setProperty("vatsinatorSettingsKey", "approach_circle_color");
    OriginToPilotLineColorButton->setProperty("vatsinatorSettingsKey", "origin_to_pilot_line_color");
    PilotToDestinationLineColorButton->setProperty("vatsinatorSettingsKey", "pilot_to_destination_line_color");
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
    }
}
