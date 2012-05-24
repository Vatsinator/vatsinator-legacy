/*
    userinterface.h
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

#include "ui/ui_userinterface.h"
#include "ui/mapwidget/mapwidget.h"
#include "singleton.h"

#ifndef NO_DEBUG
class DebugWindow;
#endif

class AirportDetailsWindow;
class ATCDetailsWindow;
class FirDetailsWindow;
class FlightDetailsWindow;
class FlightsListWindow;
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
	
	void statusBarUpdate(const QString& = "Ready");
	
	MapWidget * getGLContext() { return MapDisplay; }
	
	QProgressBar * getProgressBar() { return ProgressBar; }
	
	QLabel *	getPositionBox() { return PositionBox; }
	QLabel *	getClientsBox() { return ClientsBox; }
	
public slots:
	void quit();
	void about();
	void showMetarsWindow();
	void hideAllWindows();
	
protected:
	void closeEvent(QCloseEvent*);
	
private:
	void __setupWindow();
	void __setWindowPosition();
	void __storeWindowGeometry();
	void __restoreWindowGeometry();
	
	
#ifndef NO_DEBUG
	DebugWindow *	__debugWindow;
#endif
	
	MetarsWindow *	__metarsWindow;
	AirportDetailsWindow * __airportDetailsWindow;
	FirDetailsWindow * __firDetailsWindow;
	ATCDetailsWindow * __atcDetailsWindow;
	FlightDetailsWindow * __flightDetailsWindow;
	FlightsListWindow * __flightsListWindow;
	SettingsWindow * __settingsWindow;
	
	VatsinatorApplication &	__mother;
	
};

#endif // USERINTERFACE_H
