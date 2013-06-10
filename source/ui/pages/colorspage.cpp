/*
    colorspage.cpp
    Copyright (C) 2013  Michał Garapich michal@garapich.pl

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

#include <QtGui>

#include "colorspage.h"
#include "defines.h"

/* Default settings for ColorsPage */
namespace DefaultSettings {
  static const QColor UNSTAFFED_FIR_BORDERS_COLOR     = { 193, 193, 193 };
  static const QColor STAFFED_FIR_BORDERS_COLOR       = { 176, 32, 32 };
  static const int    STAFFED_FIR_BACKGROUND_ALPHA    = 30;
  static const QColor STAFFED_UIR_BORDERS_COLOR       = { 0, 118, 148 };
  static const int    STAFFED_UIR_BACKGROUND_ALPHA    = 30;
  static const QColor APPROACH_CIRCLE_COLOR           = { 127, 0, 0 };
  static const QColor SEAS_COLOR                      = { 188, 222, 225 };
  static const QColor LANDS_COLOR                     = { 255, 255, 255 };
  static const QColor ORIGIN_TO_PILOT_LINE_COLOR      = { 3, 116, 164 };
  static const QColor PILOT_TO_DESTINATION_LINE_COLOR = { 133, 164, 164 };
}



ColorsPage::ColorsPage(QWidget* _parent) :
    AbstractSettingsPage(_parent) {
  setupUi(this);
}

QString
ColorsPage::listElement() const {
  return tr("Colors");
}

QString
ColorsPage::listIcon() const {
  return ":/settings/preferences-colors.png";
}

QVariant
ColorsPage::get(const QString& _s) const {
  _S(staffed_fir_borders,       StaffedFirColorButton->color());
  _S(unstaffed_fir_borders,     UnstaffedFirColorButton->color());
  _S(staffed_uir_borders,       StaffedUirColorButton->color());
  _S(approach_circle,           ApproachCircleColorButton->color());
  _S(seas,                      SeasColorButton->color());
  _S(lands,                     LandsColorButton->color());
  _S(origin_to_pilot_line,      OriginToPilotLineColorButton->color());
  _S(pilot_to_destination_line, PilotToDestinationLineColorButton->color());
  
  // _S macro cannot handle these
  if (_s == "staffed_fir_background") {
    QColor tmp = StaffedFirColorButton->color();
    tmp.setAlpha(StaffedFirColorAlphaBox->value());
    return tmp;
  }
  
  if (_s == "staffed_uir_background") {
    QColor tmp = StaffedUirColorButton->color();
    tmp.setAlpha(StaffedUirColorAlphaBox->value());
    return tmp;
  }
  
  _S_END;
}

void
ColorsPage::__restore(QSettings& _s) {
  StaffedFirColorButton->setColor(
    _s.value("staffed_fir_borders", DefaultSettings::STAFFED_FIR_BORDERS_COLOR).value<QColor>());
  StaffedFirColorAlphaBox->setValue(
    _s.value("staffed_fir_alpha", DefaultSettings::STAFFED_FIR_BACKGROUND_ALPHA).toInt());
  UnstaffedFirColorButton->setColor(
    _s.value("unstaffed_fir_borders", DefaultSettings::UNSTAFFED_FIR_BORDERS_COLOR).value<QColor>());
  StaffedUirColorButton->setColor(
    _s.value("staffed_uir_borders", DefaultSettings::STAFFED_UIR_BORDERS_COLOR).value<QColor>());
  StaffedUirColorAlphaBox->setValue(
    _s.value("staffed_uir_alpha", DefaultSettings::STAFFED_UIR_BACKGROUND_ALPHA).toInt());
  ApproachCircleColorButton->setColor(
    _s.value("approach_circle", DefaultSettings::APPROACH_CIRCLE_COLOR).value<QColor>());
  SeasColorButton->setColor(
    _s.value("seas", DefaultSettings::SEAS_COLOR).value<QColor>());
  LandsColorButton->setColor(
    _s.value("lands", DefaultSettings::LANDS_COLOR).value<QColor>());
  OriginToPilotLineColorButton->setColor(
    _s.value("origin_to_pilot_line", DefaultSettings::ORIGIN_TO_PILOT_LINE_COLOR).value<QColor>());
  PilotToDestinationLineColorButton->setColor(
    _s.value("pilot_to_destination_line", DefaultSettings::PILOT_TO_DESTINATION_LINE_COLOR).value<QColor>());
}

void
ColorsPage::__save(QSettings& _s) {
  _s.setValue("staffed_fir_borders", StaffedFirColorButton->color());
  _s.setValue("staffed_fir_alpha", StaffedFirColorAlphaBox->value());
  _s.setValue("unstaffed_fir_borders", UnstaffedFirColorButton->color());
  _s.setValue("staffed_uir_borders", StaffedUirColorButton->color());
  _s.setValue("staffed_uir_alpha", StaffedUirColorAlphaBox->value());
  _s.setValue("approach_circle", ApproachCircleColorButton->color());
  _s.setValue("seas", SeasColorButton->color());
  _s.setValue("lands", LandsColorButton->color());
  _s.setValue("origin_to_pilot_line", OriginToPilotLineColorButton->color());
  _s.setValue("pilot_to_destination_line", PilotToDestinationLineColorButton->color());
}
