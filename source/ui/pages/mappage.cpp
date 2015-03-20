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

MapPage::MapPage(QWidget* parent) :
    QWidget(parent) {
  setupUi(this);
      
#ifdef Q_OS_MAC
  layout()->setSpacing(0);
#endif
  
  connect(FirFontSelectionButton,       SIGNAL(clicked()),
          this,                         SLOT(__showFirFontDialog()));
  connect(AirportFontSelectionButton,   SIGNAL(clicked()),
          this,                         SLOT(__showAirportFontDialog()));
  connect(PilotFontSelectionButton,     SIGNAL(clicked()),
          this,                         SLOT(__showPilotFontDialog()));
}

QString
MapPage::listElement() const {
  return tr("Map");
}

QString
MapPage::listIcon() const {
  return QStringLiteral(":/settings/preferences-colors.png");
}

QString
MapPage::moduleId() const {
  return QStringLiteral("map");
}

void
MapPage::update() const {
  setValue("zoom_coefficient",
           ZoomCoefficientSlider->value());
  setValue("staffed_fir_borders_color",
           StaffedFirColorButton->color());
  setValue("unstaffed_fir_borders_color",
           UnstaffedFirColorButton->color());
  setValue("staffed_uir_borders_color",
           StaffedUirColorButton->color());
  setValue("fir_font", __firFont);
  setValue("airport_font", __airportFont);
  setValue("pilot_font", __pilotFont);
  setValue("approach_circle_color",
           ApproachCircleColorButton->color());
  setValue("seas_color",
           SeasColorButton->color());
  setValue("lands_color",
           LandsColorButton->color());
  setValue("origin_to_pilot_line_color",
           OriginToPilotLineColorButton->color());
  setValue("pilot_to_destination_line_color",
           PilotToDestinationLineColorButton->color());
  
  QColor tmp = StaffedFirColorButton->color();
  tmp.setAlpha(StaffedFirColorAlphaBox->value());
  setValue("staffed_fir_background_color", tmp);
  
  tmp = StaffedUirColorButton->color();
  tmp.setAlpha(StaffedUirColorAlphaBox->value());
  
  setValue("staffed_uir_background_color", tmp);
}

void
MapPage::restore(QSettings& s, const QVariantHash& defaults) {
  QString id = moduleId() % ".";
  ZoomCoefficientSlider->setValue(
    s.value("zoom_coefficient", defaults[id % "zoom_coefficient"]).toInt());
  StaffedFirColorButton->setColor(
    s.value("staffed_fir_borders_color", defaults[id % "staffed_fir_borders_color"]).value<QColor>());
  StaffedFirColorAlphaBox->setValue(
    s.value("staffed_fir_alpha_color", defaults[id % "staffed_fir_alpha_color"]).toInt());
  UnstaffedFirColorButton->setColor(
    s.value("unstaffed_fir_borders_color", defaults[id % "unstaffed_fir_borders_color"]).value<QColor>());
  StaffedUirColorButton->setColor(
    s.value("staffed_uir_borders_color", defaults[id % "staffed_uir_borders_color"]).value<QColor>());
  StaffedUirColorAlphaBox->setValue(
    s.value("staffed_uir_alpha_color", defaults[id % "staffed_uir_alpha_color"]).toInt());
  __firFont = s.value("fir_font", defaults[id % "fir_font"]).value<QFont>();
  __airportFont = s.value("airport_font", defaults[id % "airport_font"]).value<QFont>();
  __pilotFont = s.value("pilot_font", defaults[id % "pilot_font"]).value<QFont>();
  ApproachCircleColorButton->setColor(
    s.value("approach_circle_color", defaults[id % "approach_circle_color"]).value<QColor>());
  SeasColorButton->setColor(
    s.value("seas_color", defaults[id % "seas_color"]).value<QColor>());
  LandsColorButton->setColor(
    s.value("lands_color", defaults[id % "lands_color"]).value<QColor>());
  OriginToPilotLineColorButton->setColor(
    s.value("origin_to_pilot_line_color", defaults[id % "origin_to_pilot_line_color"]).value<QColor>());
  PilotToDestinationLineColorButton->setColor(
    s.value("pilot_to_destination_line_color", defaults[id % "pilot_to_destination_line_color"]).value<QColor>());
  
  __updateFontButtons();
}

void
MapPage::save(QSettings& s) {
  s.setValue("zoom_coefficient", ZoomCoefficientSlider->value());
  s.setValue("staffed_fir_borders_color", StaffedFirColorButton->color());
  s.setValue("staffed_fir_alpha_color", StaffedFirColorAlphaBox->value());
  s.setValue("unstaffed_fir_borders_color", UnstaffedFirColorButton->color());
  s.setValue("staffed_uir_borders_color", StaffedUirColorButton->color());
  s.setValue("staffed_uir_alpha_color", StaffedUirColorAlphaBox->value());
  s.setValue("fir_font", __firFont);
  s.setValue("airport_font", __airportFont);
  s.setValue("pilot_font", __pilotFont);
  s.setValue("approach_circle_color", ApproachCircleColorButton->color());
  s.setValue("seas_color", SeasColorButton->color());
  s.setValue("lands_color", LandsColorButton->color());
  s.setValue("origin_to_pilot_line_color", OriginToPilotLineColorButton->color());
  s.setValue("pilot_to_destination_line_color", PilotToDestinationLineColorButton->color());
}

void
MapPage::__updateFontButtons() {
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
MapPage::__showFirFontDialog() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, __firFont, this);
  if (ok) {
    __firFont = font;
    __updateFontButtons();
  }
}

void
MapPage::__showAirportFontDialog() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, __airportFont, this);
  if (ok) {
    __airportFont = font;
    __updateFontButtons();
  }
}

void
MapPage::__showPilotFontDialog() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, __pilotFont, this);
  if (ok) {
    __pilotFont = font;
    __updateFontButtons();
  }
}
