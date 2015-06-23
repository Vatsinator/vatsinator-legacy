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

ViewPage::ViewPage(QWidget* parent) : WidgetSettingsModule(parent) {
  setupUi(this);
  
  connect(ShowPilotsLabelsAlwaysCheckBox, SIGNAL(stateChanged(int)),
          this,                           SLOT(__handleAlwaysCheckBox(int)));
  __handleAlwaysCheckBox(ShowPilotsLabelsAlwaysCheckBox->checkState());
  
  auto checkBoxes = findChildren<QCheckBox*>();
  for (QCheckBox* c: checkBoxes) {
    connect(c, &QCheckBox::stateChanged, this, &ViewPage::settingsChanged);
  }
  
  PilotsCheckBox->setProperty("vatsinatorSettingsKey", "pilots_layer");
  AirportsCheckBox->setProperty("vatsinatorSettingsKey", "airports_layer");
  StaffedFirsCheckBox->setProperty("vatsinatorSettingsKey", "staffed_firs");
  UnstaffedFirsCheckBox->setProperty("vatsinatorSettingsKey", "unstaffed_firs");
  InactiveAirportsCheckBox->setProperty("vatsinatorSettingsKey", "empty_airports");
  ShowPilotsLabelsAlwaysCheckBox->setProperty("vatsinatorSettingsKey", "pilot_labels.always");
  ShowPilotsLabelsWhenHoveredCheckBox->setProperty("vatsinatorSettingsKey", "pilot_labels.when_hovered");
  ShowPilotsLabelsAirportRelatedCheckBox->setProperty("vatsinatorSettingsKey", "pilot_labels.airport_related");
  AirportLabelsCheckBox->setProperty("vatsinatorSettingsKey", "airport_labels");
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
