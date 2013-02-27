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

#include "ui/ui_userinterface.h"
#include "ui/widgets/mapwidget.h"
#include "singleton.h"

#ifndef NO_DEBUG
class DebugWindow;
#endif

class AboutWindow;
class AirportDetailsWindow;
class AtcDetailsWindow;
class AtcListWindow;
class FirDetailsWindow;
class FlightDetailsWindow;
class FlightListWindow;
class MetarsWindow;
class SettingsWindow;
class QProgressBar;
class UpdateNotificationWidget;
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

  /**
   * Sets the specified message in the bottom-left bar corner
   * or puts simple "Last update" text.
   * @param text If specified, this text will be shown.
   */
  void statusBarUpdate(const QString& = "");
  
  /**
   * Toggles status bar with progress bar (bottom-left corner).
   */
  void toggleStatusBar();
  
  /**
   * The middle part of status bar - how many clients, etc etc.
   */
  void infoBarUpdate();
  
  /**
   * Sets the window on the center of the screen.
   * @param widget Window to have the position set.
   */
  static void setWindowPosition(QWidget*);

  inline MapWidget*
  getMapWidget() { return MapDisplay; }

  inline QProgressBar*
  getProgressBar() { return __progressBar; }

  inline QLabel*
  getPositionBox() { return PositionBox; }
  
  inline bool
  autoUpdatesEnabled() const { return EnableAutoUpdatesAction->isChecked(); }
  
  inline const QMenuBar *
  getMenuBar() const { return MenuBar; }

public slots:
  void quit();
  void hideAllWindows();
  void notififyAboutUpdates(bool);

protected:
  void closeEvent(QCloseEvent*);
  void resizeEvent(QResizeEvent*);

private:
  void __setupWindow();
  void __storeWindowGeometry();
  void __restoreWindowGeometry();
  
  /* Returns main window initial position */
  static QPoint __getInitialPoint();
  
  QLabel*       __statusBox;
  QProgressBar* __progressBar;


#ifndef NO_DEBUG
  DebugWindow*  __debugWindow;
#endif

  AboutWindow*          __aboutWindow;
  MetarsWindow*         __metarsWindow;
  AirportDetailsWindow* __airportDetailsWindow;
  FirDetailsWindow*     __firDetailsWindow;
  AtcDetailsWindow*     __atcDetailsWindow;
  AtcListWindow*        __atcListWindow;
  FlightDetailsWindow*  __flightDetailsWindow;
  FlightListWindow*     __flightsListWindow;
  SettingsWindow*       __settingsWindow;
  
  UpdateNotificationWidget* __updateNotification;
  
private slots:
  void __dataDownloading();
  void __dataUpdated();
  
signals:
  void autoUpdatesEnabled(bool);

};

#endif // USERINTERFACE_H
