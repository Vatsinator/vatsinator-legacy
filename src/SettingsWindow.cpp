/*
    SettingsWindow.cpp
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

#include "../include/SettingsWindow.h"

#include "../include/defines.h"

SettingsWindow::SettingsWindow(QWidget* _parent) :
		QWidget(_parent) {
	setupUi(this);
	
	__restoreSettings();
	
	connect(OKCancelButtonBox,		SIGNAL(clicked(QAbstractButton*)),
		this,				SLOT(__handleButton(QAbstractButton*)));
	
	connect(OKCancelButtonBox,		SIGNAL(accepted()),
		this,				SLOT(__hideWindow()));
	
	PilotLabelsColorButton->setColor(&__pilotsLabelsColor);
	connect(PilotLabelsColorButton,		SIGNAL(clicked(QColor*)),
		this,				SLOT(__pickColor(QColor*)));
	
	AirportLabelsColorButton->setColor(&__airportsLabelsColor);
	connect(AirportLabelsColorButton,	SIGNAL(clicked(QColor*)),
		this,				SLOT(__pickColor(QColor*)));
	
	UnstaffedFirColorButton->setColor(&__unstaffedFirBordersColor);
	connect(UnstaffedFirColorButton,	SIGNAL(clicked(QColor*)),
		this,				SLOT(__pickColor(QColor*)));
	
	StaffedFirColorButton->setColor(&__staffedFirBordersColor);
	connect(StaffedFirColorButton,		SIGNAL(clicked(QColor*)),
		this,				SLOT(__pickColor(QColor*)));
	
	StaffedUirColorButton->setColor(&__staffedUirBordersColor);
	connect(StaffedUirColorButton,		SIGNAL(clicked(QColor*)),
		this,				SLOT(__pickColor(QColor*)));
	
	ApproachCircleColorButton->setColor(&__approachCircleColor);
	connect(ApproachCircleColorButton,	SIGNAL(clicked(QColor*)),
		this,				SLOT(__pickColor(QColor*)));
	
	BackgroundColorButton->setColor(&__backgroundColor);
	connect(BackgroundColorButton,		SIGNAL(clicked(QColor*)),
		this,				SLOT(__pickColor(QColor*)));
	
	connect(ShowPilotsLabelsAlwaysCheckBox,	SIGNAL(stateChanged(int)),
		this,				SLOT(__handleAlwaysCheckBox(int)));
	
	__setButtonsColors();
	
	__setWindowPosition();

}

unsigned
SettingsWindow::getPilotsLabelsSettings() {
	if (ShowPilotsLabelsAlwaysCheckBox->checkState() == Qt::Checked)
		return ALWAYS;
	
	unsigned result = 0;
	if (ShowPilotsLabelsWhenHoveredCheckBox->checkState() == Qt::Checked)
		result |= WHEN_HOVERED;
	if (ShowPilotsLabelsAirportRelatedCheckBox->checkState() == Qt::Checked)
		result |= AIRPORT_RELATED;
	
	return result;
}

void
SettingsWindow::showWindow() {
	show();
}

void
SettingsWindow::__setButtonsColors() {
	PilotLabelsColorButton->updateColor();
	AirportLabelsColorButton->updateColor();
	UnstaffedFirColorButton->updateColor();
	StaffedFirColorButton->updateColor();
	StaffedUirColorButton->updateColor();
	ApproachCircleColorButton->updateColor();
	BackgroundColorButton->updateColor();
}

void
SettingsWindow::__saveSettings() {
	QSettings settings("Vatsinator", "Vatsinator");
	
	settings.beginGroup("Settings");
	
	settings.setValue("refreshRate", RefreshRateBox->value());
	settings.setValue("pilotsLayer", PilotsCheckBox->checkState());
	settings.setValue("airportsLayer", AirportsCheckBox->checkState());
	settings.setValue("firsLayer", FirsCheckBox->checkState());
	settings.setValue("uirsLayer", UirsCheckBox->checkState());
	settings.setValue("pilotsLabels", getPilotsLabelsSettings());
	settings.setValue("displayAirports", DisplayAirportsBox->currentIndex());

	// colors
	settings.setValue("pilotsLabelsColor", __pilotsLabelsColor);
	settings.setValue("airportsLabelsColor", __airportsLabelsColor);
	settings.setValue("unstaffedFirColor", __unstaffedFirBordersColor);
	settings.setValue("staffedFirColor", __staffedFirBordersColor);
	settings.setValue("staffedUirColor", __staffedUirBordersColor);
	settings.setValue("approachCircleColor", __approachCircleColor);
	settings.setValue("backgroundColor", __backgroundColor);
	
	settings.endGroup();
}

void
SettingsWindow::__restoreSettings() {
	QSettings settings("Vatsinator", "Vatsinator");
	
	settings.beginGroup("Settings");
	
	RefreshRateBox->setValue(settings.value("refreshRate", REFRESH_RATE).toInt());
	PilotsCheckBox->setCheckState(static_cast< Qt::CheckState >(
		settings.value("pilotsLayer", PILOTS_CHECKBOX).toInt()));
	AirportsCheckBox->setCheckState(static_cast< Qt::CheckState >(
		settings.value("airportsLayer", AIRPORTS_CHECKBOX).toInt()));
	FirsCheckBox->setCheckState(static_cast< Qt::CheckState >(
		settings.value("firsLayer", FIRS_CHECKBOX).toInt()));
	UirsCheckBox->setCheckState(static_cast< Qt::CheckState >(
		settings.value("uirsLayer", UIRS_CHECKBOX).toInt()));
	
	unsigned temp = settings.value("pilotsLabels", PILOTS_LABELS).toUInt();
	if (temp & ALWAYS)
		__handleAlwaysCheckBox(Qt::Checked);
	else {
		ShowPilotsLabelsAlwaysCheckBox->setCheckState(Qt::Unchecked);
		
		if (temp & WHEN_HOVERED)
			ShowPilotsLabelsWhenHoveredCheckBox->setCheckState(Qt::Checked);
		else
			ShowPilotsLabelsWhenHoveredCheckBox->setCheckState(Qt::Unchecked);
		
		if (temp & AIRPORT_RELATED)
			ShowPilotsLabelsAirportRelatedCheckBox->setCheckState(Qt::Checked);
		else
			ShowPilotsLabelsAirportRelatedCheckBox->setCheckState(Qt::Unchecked);
	}
	
	DisplayAirportsBox->setCurrentIndex(settings.value("displayAirports", DISPLAY_AIRPORT_BOX).toInt());
	
	__pilotsLabelsColor = settings.value("pilotsLabelsColor",
			QColor(PILOTS_LABELS_COLOR)).value< QColor >();
	__airportsLabelsColor = settings.value("airportsLabelsColor",
			QColor(AIRPORTS_LABELS_COLOR)).value< QColor >();
	__unstaffedFirBordersColor = settings.value("unstaffedFirColor",
			QColor(UNSTAFFED_FIR_BORDERS_COLOR)).value< QColor >();
	__staffedFirBordersColor = settings.value("staffedFirColor",
			QColor(STAFFED_FIR_BORDERS_COLOR)).value< QColor >();
	__staffedUirBordersColor = settings.value("staffedUirColor",
			QColor(STAFFED_UIR_BORDERS_COLOR)).value< QColor >();
	__approachCircleColor = settings.value("approachCircleColor",
			QColor(APPROACH_CIRCLE_COLOR)).value< QColor >();
	__backgroundColor = settings.value("backgroundColor",
			QColor(BACKGROUND_COLOR)).value< QColor >();
	
	settings.endGroup();
}

void
SettingsWindow::__setWindowPosition() {
	QDesktopWidget* desktop = QApplication::desktop();
	
	int screenWidth, width;
	int screenHeight, height;
	
	int x, y;
	
	QSize windowSize;
	
	screenWidth = desktop -> width();
	screenHeight = desktop -> height();
	
	windowSize = size();
	width = windowSize.width();
	height = windowSize.height();
	
	x = (screenWidth - width) / 2;
	y = (screenHeight - height) / 2;
	y -= 50;
	
	move(x, y);
}

void
SettingsWindow::__restoreDefaults() {
	RefreshRateBox->setValue(REFRESH_RATE);
	PilotsCheckBox->setCheckState(PILOTS_CHECKBOX);
	AirportsCheckBox->setCheckState(AIRPORTS_CHECKBOX);
	FirsCheckBox->setCheckState(FIRS_CHECKBOX);
	UirsCheckBox->setCheckState(UIRS_CHECKBOX);
	
	ShowPilotsLabelsAlwaysCheckBox->setCheckState(Qt::Unchecked);
	ShowPilotsLabelsWhenHoveredCheckBox->setCheckState(Qt::Checked);
	ShowPilotsLabelsAirportRelatedCheckBox->setCheckState(Qt::Checked);
	
	DisplayAirportsBox->setCurrentIndex(DISPLAY_AIRPORT_BOX);
	
	__pilotsLabelsColor = QColor(PILOTS_LABELS_COLOR);
	__airportsLabelsColor = QColor(AIRPORTS_LABELS_COLOR);
	__unstaffedFirBordersColor = QColor(UNSTAFFED_FIR_BORDERS_COLOR);
	__staffedFirBordersColor = QColor(STAFFED_FIR_BORDERS_COLOR);
	__staffedUirBordersColor = QColor(STAFFED_UIR_BORDERS_COLOR);
	__approachCircleColor = QColor(APPROACH_CIRCLE_COLOR);
	__backgroundColor = QColor(BACKGROUND_COLOR);
	
	__setButtonsColors();
}

void
SettingsWindow::__pickColor(QColor* _color) {
	QColorDialog* dialog = new QColorDialog(*_color);
	dialog->setWindowTitle("Select color");
	int result = dialog->exec();
	if (result == QDialog::Accepted)
		*_color = dialog->currentColor();
	delete dialog;
	
	__setButtonsColors();
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

void
SettingsWindow::__handleButton(QAbstractButton* _button) {
	if (OKCancelButtonBox->button(QDialogButtonBox::RestoreDefaults) == _button)
		__restoreDefaults();
}

void
SettingsWindow::__hideWindow() {
	__saveSettings();
	hide();
}

