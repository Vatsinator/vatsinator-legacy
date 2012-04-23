/*
    SettingsWindow.h
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


#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include "ui/ui_SettingsWindow.h"
#include "Singleton.h"

enum {
	WHEN_HOVERED = 1,
	AIRPORT_RELATED = 2,
	ALWAYS = 4
};

class SettingsWindow : public QWidget, public Singleton< SettingsWindow >, private Ui::SettingsWindow {
	
	Q_OBJECT
	
public:
	SettingsWindow(QWidget* = 0);
	
	unsigned getPilotsLabelsSettings();
	
	inline int getRefreshRate() { return RefreshRateBox->value(); }
	inline bool pilotsLayerOn() { return PilotsCheckBox->checkState(); }
	inline bool airportsLayerOn() { return AirportsCheckBox->checkState(); }
	inline bool firsLayerOn() { return FirsCheckBox->checkState(); }
	inline bool uirsLayerOn() { return UirsCheckBox->checkState(); }
	
	inline const QColor & getUnstaffedFirBordersColor() { return __unstaffedFirBordersColor; }
	inline const QColor & getStaffedFirBordersColor() { return __staffedFirBordersColor; }
	inline const QColor & getStaffedUirBordersColor() { return __staffedUirBordersColor; }
	inline const QColor & getApproachCircleColor() { return __approachCircleColor; }
	inline const QColor & getBackgroundColor() { return __backgroundColor; }
	
public slots:
	void showWindow();
	
private:
	void __setButtonsColors();
	
	void __saveSettings();
	void __restoreSettings();
	
	void __setWindowPosition();
	void __restoreDefaults();
	
	QColor __unstaffedFirBordersColor;
	QColor __staffedFirBordersColor;
	QColor __staffedUirBordersColor;
	QColor __approachCircleColor;
	QColor __backgroundColor;

private slots:
	void __pickColor(QColor*);
	void __handleAlwaysCheckBox(int);
	void __handleButton(QAbstractButton*);
	void __hideWindow();
	void __settingsRejected();
	
	
};

#endif // SETTINGSWINDOW_H
