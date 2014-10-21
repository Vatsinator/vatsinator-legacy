/*
    viewpage.cpp
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

#include <QtWidgets>

#include "viewpage.h"

namespace DefaultSettings {
  static const bool PILOTS_CHECKBOX           = true;
  static const bool AIRPORTS_CHECKBOX         = true;
  static const bool STAFFED_FIRS_CHECKBOX     = true;
  static const bool UNSTAFFED_FIRS_CHECKBOX   = true;
  static const bool DISPLAY_AIRPORT_LABELS    = true;
  static const bool EMPTY_AIRPORTS_CHECKBOX   = false;

  static const bool ALWAYS_CHECKBOX           = true;
  static const bool WHEN_HOVERED_CHECKBOX     = true;
  static const bool AIRPORT_RELATED_CHECKBOX  = true;
  
  static const bool AIRPORT_LABELS            = true;
}


ViewPage::ViewPage(QWidget* parent) :
    QWidget(parent) {
  setupUi(this);
  
  connect(ShowPilotsLabelsAlwaysCheckBox, SIGNAL(stateChanged(int)),
          this,                           SLOT(__handleAlwaysCheckBox(int)));
  __handleAlwaysCheckBox(ShowPilotsLabelsAlwaysCheckBox->checkState());
}

QString
ViewPage::listElement() const {
  return tr("View");
}

QString
ViewPage::listIcon() const {
  return ":/settings/preferences-view.png";
}

QString
ViewPage::moduleId() const {
  return "view";
}

void
ViewPage::update() const {
  setValue("pilots_layer",
           PilotsCheckBox->isChecked());
  setValue("airports_layer",
           AirportsCheckBox->isChecked());
  setValue("staffed_firs",
           StaffedFirsCheckBox->isChecked());
  setValue("unstaffed_firs",
           UnstaffedFirsCheckBox->isChecked());
  setValue("empty_airports",
           InactiveAirportsCheckBox->isChecked());
  setValue("pilot_labels.always",
           ShowPilotsLabelsAlwaysCheckBox->isChecked());
  setValue("pilot_labels.when_hovered",
           ShowPilotsLabelsWhenHoveredCheckBox->isChecked());
  setValue("pilot_labels.airport_related",
           ShowPilotsLabelsAirportRelatedCheckBox->isChecked());
  setValue("airport_labels",
           AlwaysRadioButton->isChecked());
}

void
ViewPage::restore(QSettings& s) {
  PilotsCheckBox->setChecked(
    s.value("pilots_layer", DefaultSettings::PILOTS_CHECKBOX).toBool());
  AirportsCheckBox->setChecked(
    s.value("airports_layer", DefaultSettings::AIRPORTS_CHECKBOX).toBool());
  StaffedFirsCheckBox->setChecked(
    s.value("staffed_firs", DefaultSettings::STAFFED_FIRS_CHECKBOX).toBool());
  UnstaffedFirsCheckBox->setChecked(
    s.value("unstaffed_firs", DefaultSettings::UNSTAFFED_FIRS_CHECKBOX).toBool());
  InactiveAirportsCheckBox->setChecked(
    s.value("empty_airports", DefaultSettings::EMPTY_AIRPORTS_CHECKBOX).toBool());
  ShowPilotsLabelsAlwaysCheckBox->setChecked(
    s.value("pilot_labels.always", DefaultSettings::ALWAYS_CHECKBOX).toBool());
  ShowPilotsLabelsWhenHoveredCheckBox->setChecked(
    s.value("pilot_labels.when_hovered", DefaultSettings::WHEN_HOVERED_CHECKBOX).toBool());
  ShowPilotsLabelsAirportRelatedCheckBox->setChecked(
    s.value("pilot_labels.airport_labels", DefaultSettings::AIRPORT_RELATED_CHECKBOX).toBool());
  
  bool airportLabels = s.value("airport_labels", DefaultSettings::AIRPORT_LABELS).toBool();
  if (airportLabels)
    AlwaysRadioButton->setChecked(true);
  else
    NeverRadioButton->setChecked(true);
}

void
ViewPage::save(QSettings& s) {
  s.setValue("pilots_layer", PilotsCheckBox->isChecked());
  s.setValue("airports_layer", AirportsCheckBox->isChecked());
  s.setValue("staffed_firs", StaffedFirsCheckBox->isChecked());
  s.setValue("unstaffed_firs", UnstaffedFirsCheckBox->isChecked());
  s.setValue("empty_airports", InactiveAirportsCheckBox->isChecked());
  s.setValue("pilot_labels.always", ShowPilotsLabelsAlwaysCheckBox->isChecked());
  s.setValue("pilot_labels.when_hovered", ShowPilotsLabelsWhenHoveredCheckBox->isChecked());
  s.setValue("pilot_labels.airport_related", ShowPilotsLabelsAirportRelatedCheckBox->isChecked());
  s.setValue("airport_labels", AlwaysRadioButton->isChecked());
}

void
ViewPage::__handleAlwaysCheckBox(int state) {
  if (state == Qt::Checked) {
    ShowPilotsLabelsAlwaysCheckBox->setCheckState(Qt::Checked);
    ShowPilotsLabelsWhenHoveredCheckBox->setCheckState(Qt::Checked);
    ShowPilotsLabelsAirportRelatedCheckBox->setCheckState(Qt::Checked);

    ShowPilotsLabelsWhenHoveredCheckBox->setEnabled(false);
    ShowPilotsLabelsAirportRelatedCheckBox->setEnabled(false);
  } else {
    ShowPilotsLabelsAlwaysCheckBox->setCheckState(Qt::Unchecked);

    ShowPilotsLabelsWhenHoveredCheckBox->setEnabled(true);
    ShowPilotsLabelsAirportRelatedCheckBox->setEnabled(true);
  }
}

