/*
    UserInterface.h
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


#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "ui_UserInterface.h"
#include "MapWidget.h"
#include "Singleton.h"

class AirportDetailsWindow;
class ATCDetailsWindow;
class FlightDetailsWindow;
class MetarsWindow;
class SettingsWindow;
class VatsinatorApplication;

class UserInterface :
		public QMainWindow,
		public Singleton< UserInterface >,
		private Ui::MainWindow {
	
	/*
	 * This class handles the Vatsinator's QT GUI interface.
	 */
	
	
	Q_OBJECT
	
public:
	UserInterface(QWidget* = 0);
	
	virtual ~UserInterface();
	
	void statusBarUpdate(const char* = "Ready.");
	
	MapWidget * getGLContext() { return MapDisplay; }
	
	QProgressBar * getProgressBar() { return ProgressBar; }
	
	QLabel *	getPositionBox() { return PositionBox; }
	QLabel *	getClientsBox() { return ClientsBox; }
	
public slots:
	void quit();
	void about();
	void showMetarsWindow();
	
private:
	void __setupWindow();
	void __setWindowPosition();
	void __storeWindowGeometry();
	void __restoreWindowGeometry();
	
	MetarsWindow *	__metarsWindow;
	AirportDetailsWindow * __airportDetailsWindow;
	ATCDetailsWindow * __atcDetailsWindow;
	FlightDetailsWindow * __flightDetailsWindow;
	SettingsWindow * __settingsWindow;
	
	VatsinatorApplication &	__mother;
	
};

#endif // USERINTERFACE_H
