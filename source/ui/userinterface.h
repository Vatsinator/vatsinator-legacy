/*
    userinterface.h
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl

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

#include <QObject>

#include "ui/widgets/mapwidget.h"
#include "singleton.h"

#ifndef NO_DEBUG
class DebugWindow;
#endif

class AboutWindow;
class Airport;
class AtcDetailsWindow;
class AtcListWindow;
class DatabaseWindow;
class FirDetailsWindow;
class FlightDetailsWindow;
class FlightListWindow;
class MetarsWindow;
class SettingsWindow;
class VatsinatorWindow;

class UserInterface : public QObject, public Singleton<UserInterface> {

  /*
   * This class manager the Vatsinator's GUI.
   */

  Q_OBJECT

public:
  UserInterface();
  virtual ~UserInterface();
  
public:
  void init();
  
public slots:
  /**
   * Reports fatal error to user.
   * After showing the error message box, the application
   * will terminate.
   * 
   * @param msg Message to be shown.
   */
  void fatal(const QString&);
  
  /**
   * Reports warning to user.
   * 
   * @param msg Warning message.
   */
  void warning(const QString&);
  
  void showAppRestartDialog();
  void showVatsimMessage(const QString&);
  
  void showDetailsWindow(const Airport*);

private slots:
  void __statusError();
  void __dataError();
  void __showNewVersionDialog();
  
private:

#ifndef NO_DEBUG
  DebugWindow*  __debugWindow;
#endif

  AboutWindow*          __aboutWindow;
  MetarsWindow*         __metarsWindow;
  DatabaseWindow*       __databaseWindow;
  FirDetailsWindow*     __firDetailsWindow;
  AtcDetailsWindow*     __atcDetailsWindow;
  AtcListWindow*        __atcListWindow;
  FlightDetailsWindow*  __flightDetailsWindow;
  FlightListWindow*     __flightsListWindow;
  SettingsWindow*       __settingsWindow;
  VatsinatorWindow*     __vatsinatorWindow;

};

#endif // USERINTERFACE_H
