/*
    settingswindow.h
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

#include "ui/ui_settingswindow.h"
#include "singleton.h"

class SettingsManager;

class SettingsWindow :
		public QWidget,
		public Singleton< SettingsWindow >,
		private Ui::SettingsWindow {
	
	Q_OBJECT
	
	friend class SettingsManager;
	
public:
	SettingsWindow(QWidget* = 0);
	
signals:
	void restoreDefaults();
	
public slots:
	void show();
	
private:
	
	/* There is nothing to explain */
	void __setWindowPosition();
	
	SettingsManager * __mySettingsManager;
	
private slots:
	/* Reads settings from the SettingsManager instance
	 * and updated the window */
	void __updateWindow();
	
	/* For "Restore defaults" button */
	void __handleButton(QAbstractButton*);
	
	/* Disables checkboxes */
	void __handleAlwaysCheckBox(int);
	
};

#endif // SETTINGSWINDOW_H
