/*
    SettingsManager.cpp
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

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

#include "ui/windows/SettingsWindow.h"

#include "SettingsManager.h"
#include "defines.h"

#include "defaultsettings.h" // for restoring defaults

SettingsManager::SettingsManager(QObject* _parent) :
		QObject(_parent) {
	__restoreSettings();
}

void
SettingsManager::init() {
	__mySettingsWindow = SettingsWindow::GetSingletonPtr();
	connect(__mySettingsWindow->OKCancelButtonBox,	SIGNAL(accepted()),
		this,					SLOT(__updateSettings()));
	connect(__mySettingsWindow,			SIGNAL(restoreDefaults()),
		this,					SLOT(__restoreDefaults()));
}


void
SettingsManager::__saveSettings() {
	QSettings settings("Vatsinator", "Vatsinator");
	
	settings.beginGroup("Settings");
	
	settings.setValue("refreshRate",	__refreshRate);
	settings.setValue("antyaliasing",	__antyaliasing);
	settings.setValue("pilotsLayer",	__displayLayers.pilots);
	settings.setValue("airportsLayer",	__displayLayers.airports);
	settings.setValue("firsLayer",		__displayLayers.firs);
	settings.setValue("uirsLayer",		__displayLayers.uirs);
	
	settings.setValue("pilotsLabelsDisplayPolicy",	__pilotsLabelsDisplayPolicy);
	
	settings.setValue("unstaffedFirBordersColor",	__unstaffedFirBordersColor);
	settings.setValue("staffedFirBordersColor",	__staffedFirBordersColor);
	settings.setValue("staffedUirBordersColor",	__staffedUirBordersColor);
	settings.setValue("approachCircleColor",	__approachCircleColor);
	settings.setValue("backgroundColor",		__backgroundColor);
	
	settings.endGroup();
}

void
SettingsManager::__restoreSettings() {
	QSettings settings("Vatsinator", "Vatsinator");
	
	settings.beginGroup("Settings");
	
	__refreshRate = settings.value("refreshRate", DefaultSettings::REFRESH_RATE).toInt();
	__antyaliasing = settings.value("antyaliasing", DefaultSettings::ANTYALIASING).toBool();
	__displayLayers.pilots = settings.value("pilotsLayer", DefaultSettings::PILOTS_CHECKBOX).toBool();
	__displayLayers.airports = settings.value("airportsLayer", DefaultSettings::AIRPORTS_CHECKBOX).toBool();
	__displayLayers.firs = settings.value("firsLayer", DefaultSettings::FIRS_CHECKBOX).toBool();
	__displayLayers.uirs = settings.value("uirsLayer", DefaultSettings::UIRS_CHECKBOX).toBool();
	
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
	__staffedUirBordersColor = settings.value("staffedUirBordersColor",
						  DefaultSettings::STAFFED_UIR_BORDERS_COLOR).value<QColor>();
	__approachCircleColor = settings.value("approachCircleColor",
					       DefaultSettings::APPROACH_CIRCLE_COLOR).value<QColor>();
	__backgroundColor = settings.value("backgroundColor",
					   DefaultSettings::BACKGROUND_COLOR).value<QColor>();
	   
#ifndef NO_DEBUG
	qDebug() << "Antyaliasing " << (__antyaliasing ? "on" : "off");
#endif
	
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
	__antyaliasing = static_cast< bool >(__mySettingsWindow->AntyaliasingCheckBox->checkState());
	__displayLayers.pilots = static_cast< bool >(__mySettingsWindow->PilotsCheckBox->checkState());
	__displayLayers.airports = static_cast< bool >(__mySettingsWindow->AirportsCheckBox->checkState());
	__displayLayers.firs = static_cast< bool >(__mySettingsWindow->FirsCheckBox->checkState());
	__displayLayers.uirs = static_cast< bool >(__mySettingsWindow->UirsCheckBox->checkState());
	
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
	__staffedUirBordersColor = __mySettingsWindow->StaffedUirColorButton->getColor();
	__approachCircleColor = __mySettingsWindow->ApproachCircleColorButton->getColor();
	__backgroundColor = __mySettingsWindow->BackgroundColorButton->getColor();
	
	__saveSettings();
	
	emit settingsChanged();
}

void
SettingsManager::__restoreDefaults() {
	__refreshRate = DefaultSettings::REFRESH_RATE;
	__displayLayers.pilots = DefaultSettings::PILOTS_CHECKBOX;
	__displayLayers.airports = DefaultSettings::AIRPORTS_CHECKBOX;
	__displayLayers.firs = DefaultSettings::FIRS_CHECKBOX;
	__displayLayers.uirs = DefaultSettings::UIRS_CHECKBOX;
	
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
	__staffedUirBordersColor = DefaultSettings::STAFFED_UIR_BORDERS_COLOR;
	__approachCircleColor = DefaultSettings::APPROACH_CIRCLE_COLOR;
	__backgroundColor = DefaultSettings::BACKGROUND_COLOR;
	
	__clearEntries();
	
	__saveSettings();
	
	emit settingsRestored();
	emit settingsChanged();
}



