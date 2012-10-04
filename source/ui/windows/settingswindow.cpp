/*
    settingswindow.cpp
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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

#include "settings/languagemanager.h"
#include "settings/settingsmanager.h"

#include "ui/widgets/mapwidget.h"

#include "ui/userinterface.h"

#include "settingswindow.h"
#include "defines.h"

SettingsWindow::SettingsWindow(QWidget* _parent) :
    QWidget(_parent),
    __mySettingsManager(SettingsManager::GetSingletonPtr()) {
  setupUi(this);
  UserInterface::setWindowPosition(this);
  LanguageComboBox->addItems(LanguageManager::GetSingleton().getAllLanguages());

  connect(OKCancelButtonBox,  SIGNAL(clicked(QAbstractButton*)),
          this,     SLOT(__handleButton(QAbstractButton*)));
  connect(OKCancelButtonBox,  SIGNAL(accepted()),
          this,     SLOT(hide()));
  connect(ShowPilotsLabelsAlwaysCheckBox, SIGNAL(stateChanged(int)),
          this,     SLOT(__handleAlwaysCheckBox(int)));
  connect(__mySettingsManager,  SIGNAL(settingsRestored()),
          this,     SLOT(__updateWindow()));
}

void
SettingsWindow::show() {
  __updateWindow();
  QWidget::show();
}

void
SettingsWindow::__updateWindow() {
  RefreshRateBox->setValue(__mySettingsManager->getRefreshRate());
  RefreshMetarsCheckBox->setChecked(__mySettingsManager->refreshMetars());
  AntyaliasingCheckBox->setChecked(__mySettingsManager->hasAntyaliasing());
  EnableCachingCheckBox->setChecked(__mySettingsManager->cacheEnabled());
  ZoomCoefficientSlider->setValue(__mySettingsManager->getZoomCoefficient());
  PilotsCheckBox->setChecked(__mySettingsManager->getDisplayLayersPolicy().pilots);
  AirportsCheckBox->setChecked(__mySettingsManager->getDisplayLayersPolicy().airports);
  StaffedFirsCheckBox->setChecked(__mySettingsManager->getDisplayLayersPolicy().staffedFirs);
  UnstaffedFirsCheckBox->setChecked(__mySettingsManager->getDisplayLayersPolicy().unstaffedFirs);
  InactiveAirportsCheckBox->setChecked(__mySettingsManager->getDisplayLayersPolicy().emptyAirports);

  if (__mySettingsManager->displayAirportLabels()) {
    AlwaysRadioButton->setChecked(true);
    NeverRadioButton->setChecked(false);
  } else {
    AlwaysRadioButton->setChecked(false);
    NeverRadioButton->setChecked(true);
  }

  if (__mySettingsManager->displayPilotsLabelsAlways())
    __handleAlwaysCheckBox(Qt::Checked);
  else {
    __handleAlwaysCheckBox(Qt::Unchecked);

    if (__mySettingsManager->displayPilotsLabelsWhenHovered())
      ShowPilotsLabelsWhenHoveredCheckBox->setCheckState(Qt::Checked);
    else
      ShowPilotsLabelsWhenHoveredCheckBox->setCheckState(Qt::Unchecked);

    if (__mySettingsManager->displayPilotsLabelsAirportRelated())
      ShowPilotsLabelsAirportRelatedCheckBox->setCheckState(Qt::Checked);
    else
      ShowPilotsLabelsAirportRelatedCheckBox->setCheckState(Qt::Unchecked);
  }

  UnstaffedFirColorButton->setColor(__mySettingsManager->getUnstaffedFirBordersColor());
  StaffedFirColorButton->setColor(__mySettingsManager->getStaffedFirBordersColor());
  StaffedFirColorAlphaBox->setValue(__mySettingsManager->getStaffedFirBackgroundColor().alpha());
  StaffedUirColorButton->setColor(__mySettingsManager->getStaffedUirBordersColor());
  StaffedUirColorAlphaBox->setValue(__mySettingsManager->getStaffedUirBackgroundColor().alpha());
  ApproachCircleColorButton->setColor(__mySettingsManager->getApproachCircleColor());
  SeasColorButton->setColor(__mySettingsManager->getSeasColor());
  LandsColorButton->setColor(__mySettingsManager->getLandsColor());
  OriginToPilotLineColorButton->setColor(__mySettingsManager->getOriginToPilotLineColor());
  PilotToDestinationLineColorButton->setColor(__mySettingsManager->getPilotToDestinationLineColor());
  
  LanguageComboBox->setCurrentIndex(
      LanguageManager::GetSingleton().getLanguageId(__mySettingsManager->getLanguage())
    );
}


void
SettingsWindow::__handleButton(QAbstractButton* _button) {
  if (OKCancelButtonBox->button(QDialogButtonBox::RestoreDefaults) == _button) {
    emit restoreDefaults();
  } else if (OKCancelButtonBox->button(QDialogButtonBox::Apply) == _button) {
    emit settingsApplied();
  }
}

void
SettingsWindow::__handleAlwaysCheckBox(int _state) {
  if (_state == Qt::Checked) {
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




