/*
    settingsmanager.cpp
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

#include "ui/windows/settingswindow.h"

#include "vatsinatorapplication.h"

#include "settingsmanager.h"
#include "defines.h"

#include "defaultsettings.h" // for restoring defaults

SettingsManager::SettingsManager(QObject* _parent) :
    QObject(_parent) {
  __restoreSettings();
}

void
SettingsManager::init() {
  __mySettingsWindow = SettingsWindow::GetSingletonPtr();
  connect(__mySettingsWindow->OKCancelButtonBox,  SIGNAL(accepted()),
          this,                                   SLOT(__updateSettings()));
  connect(__mySettingsWindow,                     SIGNAL(restoreDefaults()),
          this,                                   SLOT(__restoreDefaults()));
}

void
SettingsManager::__saveSettings() {
  QSettings settings("Vatsinator", "Vatsinator");

  settings.beginGroup("Settings");

  settings.setValue("refreshRate",                 __refreshRate);
  settings.setValue("metarsRefresh",               __metarsRefresh);
  settings.setValue("antyaliasing",                __antyaliasing);
  settings.setValue("pilotsLayer",                 __displayLayers.pilots);
  settings.setValue("airportsLayer",               __displayLayers.airports);
  settings.setValue("staffedFirsLayer",            __displayLayers.staffedFirs);
  settings.setValue("unstaffedFirsLayer",          __displayLayers.unstaffedFirs);
  settings.setValue("emptyAirportsLayer",          __displayLayers.emptyAirports);
  settings.setValue("displayApLabels",             __displayAirportLabels);

  settings.setValue("pilotsLabelsDisplayPolicy",   __pilotsLabelsDisplayPolicy);

  settings.setValue("unstaffedFirBordersColor",    __unstaffedFirBordersColor);
  settings.setValue("staffedFirBordersColor",      __staffedFirBordersColor);
  settings.setValue("staffedFirBackgroundAlpha",   __staffedFirBackgroundAlpha);
  settings.setValue("staffedUirBordersColor",      __staffedUirBordersColor);
  settings.setValue("staffedUirBackgroundAlpha",   __staffedUirBackgroundAlpha);
  settings.setValue("approachCircleColor",         __approachCircleColor);
  settings.setValue("seasColor",                   __seasColor);
  settings.setValue("landsColor",                  __landsColor);
  settings.setValue("originToPilotLineColor",      __originToPilotLineColor);
  settings.setValue("pilotToDestinationLineColor", __pilotToDestinationLineColor);

  settings.endGroup();
}

void
SettingsManager::__restoreSettings() {
  QSettings settings("Vatsinator", "Vatsinator");

  settings.beginGroup("Settings");

  __refreshRate = settings.value("refreshRate", DefaultSettings::REFRESH_RATE).toInt();
  __metarsRefresh = settings.value("metarsRefresh", DefaultSettings::METARS_REFRESH).toBool();
  __antyaliasing = settings.value("antyaliasing", DefaultSettings::ANTYALIASING).toBool();
  __displayLayers.pilots = settings.value("pilotsLayer", DefaultSettings::PILOTS_CHECKBOX).toBool();
  __displayLayers.airports = settings.value("airportsLayer", DefaultSettings::AIRPORTS_CHECKBOX).toBool();
  __displayLayers.staffedFirs = settings.value("staffedFirsLayer", DefaultSettings::STAFFED_FIRS_CHECKBOX).toBool();
  __displayLayers.unstaffedFirs = settings.value("unstaffedFirsLayer", DefaultSettings::UNSTAFFED_FIRS_CHECKBOX).toBool();
  __displayLayers.emptyAirports = settings.value("emptyAirportsLayer", DefaultSettings::EMPTY_AIRPORTS_CHECKBOX).toBool();
  __displayAirportLabels = settings.value("displayApLabels", DefaultSettings::DISPLAY_AIRPORT_LABELS).toBool();

  /* Determine the default pilots' labels display policy settings */
  unsigned temp = 0;

  if (DefaultSettings::ALWAYS_CHECKBOX)
    temp = ALWAYS;
  else {
    if (DefaultSettings::WHEN_HOVERED_CHECKBOX)
      temp |= WHEN_HOVERED;

    if (DefaultSettings::AIRPORT_RELATED_CHECKBOX)
      temp |= AIRPORT_RELATED;
  }

  __pilotsLabelsDisplayPolicy = settings.value("pilotsLabelsDisplayPolicy", temp).toUInt();

  __unstaffedFirBordersColor = settings.value("unstaffedFirBordersColor",
                                              DefaultSettings::UNSTAFFED_FIR_BORDERS_COLOR).value<QColor>();
  __staffedFirBordersColor = settings.value("staffedFirBordersColor",
                                            DefaultSettings::STAFFED_FIR_BORDERS_COLOR).value<QColor>();
  __staffedFirBackgroundAlpha = settings.value("staffedFirBackgroundAlpha",
                                               DefaultSettings::STAFFED_FIR_BACKGROUND_ALPHA).toInt();
  __staffedFirBackgroundColor = __staffedFirBordersColor;
  __staffedFirBackgroundColor.setAlpha(__staffedFirBackgroundAlpha);

  __staffedUirBordersColor = settings.value("staffedUirBordersColor",
                                            DefaultSettings::STAFFED_UIR_BORDERS_COLOR).value<QColor>();
  __staffedUirBackgroundAlpha = settings.value("staffedUirBackgroundAlpha",
                                               DefaultSettings::STAFFED_UIR_BACKGROUND_ALPHA).toInt();
  __staffedUirBackgroundColor = __staffedUirBordersColor;
  __staffedUirBackgroundColor.setAlpha(__staffedUirBackgroundAlpha);

  __approachCircleColor = settings.value("approachCircleColor",
                                         DefaultSettings::APPROACH_CIRCLE_COLOR).value<QColor>();
  __seasColor = settings.value("seasColor",
                               DefaultSettings::SEAS_COLOR).value<QColor>();
  __landsColor = settings.value("landsColor",
                                DefaultSettings::LANDS_COLOR).value<QColor>();
  __originToPilotLineColor = settings.value("originToPilotLineColor",
                                            DefaultSettings::ORIGIN_TO_PILOT_LINE_COLOR).value<QColor>();
  __pilotToDestinationLineColor = settings.value("pilotToDestinationLineColor",
                                                 DefaultSettings::PILOT_TO_DESTINATION_LINE_COLOR).value<QColor>();

  settings.endGroup();

  emit settingsChanged();
}

void
SettingsManager::__clearEntries() {
  QSettings settings("Vatsinator", "Vatsinator");

  settings.beginGroup("Settings");
  settings.remove("");
  settings.endGroup();
}

void
SettingsManager::__updateSettings() {
  /* Get settings state from SettingsWindow */
  __refreshRate = __mySettingsWindow->RefreshRateBox->value();
  __metarsRefresh = static_cast< bool >(__mySettingsWindow->RefreshMetarsCheckBox->checkState());
  __antyaliasing = static_cast< bool >(__mySettingsWindow->AntyaliasingCheckBox->checkState());
  __displayLayers.pilots = static_cast< bool >(__mySettingsWindow->PilotsCheckBox->checkState());
  __displayLayers.airports = static_cast< bool >(__mySettingsWindow->AirportsCheckBox->checkState());
  __displayLayers.staffedFirs = static_cast< bool >(__mySettingsWindow->StaffedFirsCheckBox->checkState());
  __displayLayers.unstaffedFirs = static_cast< bool >(__mySettingsWindow->UnstaffedFirsCheckBox->checkState());
  __displayLayers.emptyAirports = static_cast< bool >(__mySettingsWindow->InactiveAirportsCheckBox->checkState());

  if (__mySettingsWindow->AlwaysRadioButton->isChecked())
    __displayAirportLabels = true;
  else
    __displayAirportLabels = false;

  /* Obtain pilots labels display policy */
  __pilotsLabelsDisplayPolicy = 0;

  if (__mySettingsWindow->ShowPilotsLabelsAlwaysCheckBox->checkState() == Qt::Checked)
    __pilotsLabelsDisplayPolicy = ALWAYS;
  else {
    if (__mySettingsWindow->ShowPilotsLabelsWhenHoveredCheckBox->checkState() == Qt::Checked)
      __pilotsLabelsDisplayPolicy |= WHEN_HOVERED;

    if (__mySettingsWindow->ShowPilotsLabelsAirportRelatedCheckBox->checkState() == Qt::Checked)
      __pilotsLabelsDisplayPolicy |= AIRPORT_RELATED;
  }

  __unstaffedFirBordersColor = __mySettingsWindow->UnstaffedFirColorButton->getColor();
  __staffedFirBordersColor = __mySettingsWindow->StaffedFirColorButton->getColor();
  __staffedFirBackgroundAlpha = __mySettingsWindow->StaffedFirColorAlphaBox->value();
  __staffedFirBackgroundColor = __staffedFirBordersColor;
  __staffedFirBackgroundColor.setAlpha(__staffedFirBackgroundAlpha);

  __staffedUirBordersColor = __mySettingsWindow->StaffedUirColorButton->getColor();
  __staffedUirBackgroundAlpha = __mySettingsWindow->StaffedUirColorAlphaBox->value();
  __staffedUirBackgroundColor = __staffedUirBordersColor;
  __staffedUirBackgroundColor.setAlpha(__staffedUirBackgroundAlpha);

  __approachCircleColor = __mySettingsWindow->ApproachCircleColorButton->getColor();
  __seasColor = __mySettingsWindow->SeasColorButton->getColor();
  __landsColor = __mySettingsWindow->LandsColorButton->getColor();
  __originToPilotLineColor = __mySettingsWindow->OriginToPilotLineColorButton->getColor();
  __pilotToDestinationLineColor = __mySettingsWindow->PilotToDestinationLineColorButton->getColor();

  __saveSettings();

  emit settingsChanged();
}

void
SettingsManager::__restoreDefaults() {
  __refreshRate = DefaultSettings::REFRESH_RATE;
  __metarsRefresh = DefaultSettings::METARS_REFRESH;
  __antyaliasing = DefaultSettings::ANTYALIASING;
  __displayLayers.pilots = DefaultSettings::PILOTS_CHECKBOX;
  __displayLayers.airports = DefaultSettings::AIRPORTS_CHECKBOX;
  __displayLayers.staffedFirs = DefaultSettings::STAFFED_FIRS_CHECKBOX;
  __displayLayers.unstaffedFirs = DefaultSettings::UNSTAFFED_FIRS_CHECKBOX;
  __displayLayers.emptyAirports = DefaultSettings::EMPTY_AIRPORTS_CHECKBOX;
  __displayAirportLabels = DefaultSettings::DISPLAY_AIRPORT_LABELS;

  __pilotsLabelsDisplayPolicy = 0;

  if (DefaultSettings::ALWAYS_CHECKBOX)
    __pilotsLabelsDisplayPolicy = ALWAYS;
  else {
    if (DefaultSettings::WHEN_HOVERED_CHECKBOX)
      __pilotsLabelsDisplayPolicy |= WHEN_HOVERED;

    if (DefaultSettings::AIRPORT_RELATED_CHECKBOX)
      __pilotsLabelsDisplayPolicy |= AIRPORT_RELATED;
  }

  __unstaffedFirBordersColor = DefaultSettings::UNSTAFFED_FIR_BORDERS_COLOR;
  __staffedFirBordersColor = DefaultSettings::STAFFED_FIR_BORDERS_COLOR;
  __staffedFirBackgroundAlpha = DefaultSettings::STAFFED_FIR_BACKGROUND_ALPHA;
  __staffedFirBackgroundColor = __staffedFirBordersColor;
  __staffedFirBackgroundColor.setAlpha(__staffedFirBackgroundAlpha);

  __staffedUirBordersColor = DefaultSettings::STAFFED_UIR_BORDERS_COLOR;
  __staffedUirBackgroundAlpha = DefaultSettings::STAFFED_UIR_BACKGROUND_ALPHA;
  __staffedUirBackgroundColor = __staffedUirBordersColor;
  __staffedUirBackgroundColor.setAlpha(__staffedUirBackgroundAlpha);

  __approachCircleColor = DefaultSettings::APPROACH_CIRCLE_COLOR;
  __seasColor = DefaultSettings::SEAS_COLOR;
  __landsColor = DefaultSettings::LANDS_COLOR;
  __originToPilotLineColor = DefaultSettings::ORIGIN_TO_PILOT_LINE_COLOR;
  __pilotToDestinationLineColor = DefaultSettings::PILOT_TO_DESTINATION_LINE_COLOR;

  __clearEntries();

  __saveSettings();

  VatsinatorApplication::log("Defaults restored.");

  emit settingsRestored();
  emit settingsChanged();
}



