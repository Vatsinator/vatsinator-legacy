/*
    UserInterface.cpp
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

#include <cstdlib>

#include <QtGui>

#include "../include/UserInterface.h"

#include "../include/AirportDetailsWindow.h"
#include "../include/ATCDetailsWindow.h"
#include "../include/MetarsWindow.h"
#include "../include/SettingsWindow.h"
#include "../include/VatsinatorApplication.h"
#include "../include/VatsimDataHandler.h"
#include "../include/FlightDetailsWindow.h"

#include "../include/defines.h"

UserInterface::UserInterface(QWidget* _parent) :
		QMainWindow(_parent),
		__atcDetailsWindow(new ATCDetailsWindow()),
		__flightDetailsWindow(new FlightDetailsWindow()),
		__metarsWindow(new MetarsWindow()),
		__settingsWindow(new SettingsWindow()),
		__airportDetailsWindow(new AirportDetailsWindow()),
		__mother(VatsinatorApplication::GetSingleton()) {
	__setupWindow();
	__setWindowPosition();
	__restoreWindowGeometry();
	
	connect(ActionExit,	SIGNAL(triggered()),
		this,		SLOT(quit()));
	connect(ActionAbout,	SIGNAL(triggered()),
		this,		SLOT(about()));
	connect(ActionMetar,	SIGNAL(triggered()),
		__metarsWindow,	SLOT(showWindow()));
	connect(ActionRefresh,	SIGNAL(triggered()),
		&__mother,	SLOT(refreshData()));
	connect(ActionPreferences,	SIGNAL(triggered()),
		SettingsWindow::GetSingletonPtr(),	SLOT(showWindow()));
	
	statusBarUpdate();
	
	ProgressBar->reset();	
}

UserInterface::~UserInterface() {
	__storeWindowGeometry();
	delete __airportDetailsWindow;
	delete __flightDetailsWindow;
	delete __atcDetailsWindow;
	delete __settingsWindow;
	delete __metarsWindow;
}

void
UserInterface::statusBarUpdate(const char* _message) {
	StatusBox->setText(tr(_message));
}

void
UserInterface::quit() {
	VatsinatorApplication::GetSingleton().quit();
}

void
UserInterface::about() {
	QMessageBox::about(this, "About Vatsinator",
			"<center>"
			"Vatsinator<br />"
			"The simple Vatsim monitor<br />"
			"Version " VATSINATOR_VERSION "<br />"
			"Copyright 2012"
			"</center>"
	);
	statusBarUpdate();
}

void
UserInterface::showMetarsWindow() {
	__metarsWindow->show();
}

void
UserInterface::__setupWindow() {
	setupUi(this);
}

void
UserInterface::__setWindowPosition() {
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
UserInterface::__storeWindowGeometry() {
	QSettings settings("Vatsinator", "Vatsinator");
	
	settings.beginGroup("MainWindow");
	
	settings.setValue("geometry", saveGeometry());
	settings.setValue("savestate", saveState());
	settings.setValue("maximized", isMaximized());
	if (!isMaximized()) {
		settings.setValue("position", pos());
		settings.setValue("size", size());
	}
	
	settings.endGroup();
}

void
UserInterface::__restoreWindowGeometry() {
	QSettings settings("Vatsinator", "Vatsinator");
	
	settings.beginGroup("MainWindow");
	restoreGeometry(settings.value( "geometry", saveGeometry() ).toByteArray());
	restoreState(settings.value( "savestate", saveState() ).toByteArray());
	move(settings.value( "position", pos() ).toPoint());
	resize(settings.value( "size", size() ).toSize());
	if ( settings.value( "maximized", isMaximized() ).toBool() )
		showMaximized();
	settings.endGroup();
}

