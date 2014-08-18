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

#include "mappage.h"

/* Default settings for MapPage */
namespace DefaultSettings {
  static const int    ZOOM_COEFFICIENT                = 30;
  static const QColor UNSTAFFED_FIR_BORDERS_COLOR     = { 193, 193, 193 };
  static const QColor STAFFED_FIR_BORDERS_COLOR       = { 176, 32, 32 };
  static const int    STAFFED_FIR_BACKGROUND_ALPHA    = 30;
  static const QColor STAFFED_UIR_BORDERS_COLOR       = { 0, 118, 148 };
  static const int    STAFFED_UIR_BACKGROUND_ALPHA    = 30;
  static const QFont  FIR_FONT                        = QFont("Verdana", 9);
  static const QFont  AIRPORT_FONT                    = QFont("Verdana", 8);
  static const QFont  PILOT_FONT                      = QFont("Verdana", 8);
  static const QColor APPROACH_CIRCLE_COLOR           = { 64, 127, 91 };
  static const QColor SEAS_COLOR                      = { 188, 222, 225 };
  static const QColor LANDS_COLOR                     = { 255, 255, 255 };
  static const QColor ORIGIN_TO_PILOT_LINE_COLOR      = { 3, 116, 164 };
  static const QColor PILOT_TO_DESTINATION_LINE_COLOR = { 133, 164, 164 };
}

MapPage::MapPage(QWidget* _parent) :
    QWidget(_parent) {
  setupUi(this);
  
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
  return ":/settings/preferences-colors.png";
}

QString
MapPage::moduleId() const {
  return "map";
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
MapPage::restore(QSettings& _s) {
  ZoomCoefficientSlider->setValue(
    _s.value("zoom_coefficient", DefaultSettings::ZOOM_COEFFICIENT).toInt());
  StaffedFirColorButton->setColor(
    _s.value("staffed_fir_borders_color", DefaultSettings::STAFFED_FIR_BORDERS_COLOR).value<QColor>());
  StaffedFirColorAlphaBox->setValue(
    _s.value("staffed_fir_alpha_color", DefaultSettings::STAFFED_FIR_BACKGROUND_ALPHA).toInt());
  UnstaffedFirColorButton->setColor(
    _s.value("unstaffed_fir_borders_color", DefaultSettings::UNSTAFFED_FIR_BORDERS_COLOR).value<QColor>());
  StaffedUirColorButton->setColor(
    _s.value("staffed_uir_borders_color", DefaultSettings::STAFFED_UIR_BORDERS_COLOR).value<QColor>());
  StaffedUirColorAlphaBox->setValue(
    _s.value("staffed_uir_alpha_color", DefaultSettings::STAFFED_UIR_BACKGROUND_ALPHA).toInt());
  __firFont = _s.value("fir_font", DefaultSettings::FIR_FONT).value<QFont>();
  __airportFont = _s.value("airport_font", DefaultSettings::AIRPORT_FONT).value<QFont>();
  __pilotFont = _s.value("pilot_font", DefaultSettings::PILOT_FONT).value<QFont>();
  ApproachCircleColorButton->setColor(
    _s.value("approach_circle_color", DefaultSettings::APPROACH_CIRCLE_COLOR).value<QColor>());
  SeasColorButton->setColor(
    _s.value("seas_color", DefaultSettings::SEAS_COLOR).value<QColor>());
  LandsColorButton->setColor(
    _s.value("lands_color", DefaultSettings::LANDS_COLOR).value<QColor>());
  OriginToPilotLineColorButton->setColor(
    _s.value("origin_to_pilot_line_color", DefaultSettings::ORIGIN_TO_PILOT_LINE_COLOR).value<QColor>());
  PilotToDestinationLineColorButton->setColor(
    _s.value("pilot_to_destination_line_color", DefaultSettings::PILOT_TO_DESTINATION_LINE_COLOR).value<QColor>());
  
  __updateFontButtons();
}

void
MapPage::save(QSettings& _s) {
  _s.setValue("zoom_coefficient", ZoomCoefficientSlider->value());
  _s.setValue("staffed_fir_borders_color", StaffedFirColorButton->color());
  _s.setValue("staffed_fir_alpha_color", StaffedFirColorAlphaBox->value());
  _s.setValue("unstaffed_fir_borders_color", UnstaffedFirColorButton->color());
  _s.setValue("staffed_uir_borders_color", StaffedUirColorButton->color());
  _s.setValue("staffed_uir_alpha_color", StaffedUirColorAlphaBox->value());
  _s.setValue("fir_font", __firFont);
  _s.setValue("airport_font", __airportFont);
  _s.setValue("pilot_font", __pilotFont);
  _s.setValue("approach_circle_color", ApproachCircleColorButton->color());
  _s.setValue("seas_color", SeasColorButton->color());
  _s.setValue("lands_color", LandsColorButton->color());
  _s.setValue("origin_to_pilot_line_color", OriginToPilotLineColorButton->color());
  _s.setValue("pilot_to_destination_line_color", PilotToDestinationLineColorButton->color());
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
