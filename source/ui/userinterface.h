/*
    userinterface.h
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

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

class Airport;
class Client;
class Fir;
class QEvent;
class NotificationEvent;

/**
 * The UserInterface is an abstract class that specifies common behaviour
 * independent from the desktop mode. Views, notifications, etc. are different
 * on desktop and on mobile platforms, but they should be handled properly
 * in order to look native. 
 */
class UserInterface : public QObject {
  Q_OBJECT
  
signals:
  void initialized();

public:
  /**
   * The default constructor passes _parent_ to QObject.
   */
  UserInterface(QObject* parent = nullptr);
  
  virtual ~UserInterface() = default;
  
  /**
   * Initializes UI components. Must emit the initialized() signal when
   * all classes are instantiated.
   */
  virtual void initialize() = 0;
  
  /**
   * Custom events handling.
   */
  bool event(QEvent* event) override;
  
public slots:
  /**
   * Reports critical error to user.
   * This should let user know that he has corrupted the application, his
   * device is not supported, etc.
   * 
   * \param message Message to be shown.
   */
  virtual void fatal(const QString& message) = 0;
  
  /**
   * Reports warning to user.
   * It should warn user about corrupted file that the application depends on,
   * missing file, etc. User should just click "OK" and continue, not
   * necessarily without errors.
   * 
   * \param message Warning message.
   */
  virtual void warning(const QString& message) = 0;
  
  /**
   * Show Vatsim status fetch error notification.
   * Status.txt file is fetched on the startup of Vatsinator. If the download
   * process fails, it means that either Vatsim servers are not available at
   * all or user does not have connection to the internet.
   * Call vApp()->vatsimData()->requestDataUpdate() to retry.
   */
  virtual void statusError() = 0;
  
  /**
   * Show Vatsim data fetch error notification.
   * When this function is called it means that Vatsinator tried a couple
   * of times to refresh Vatsim status, but it was impossible or user choose
   * manual data updates and the download process failed. Show the appropriate
   * notification and let user decide whether he wants to retry the operation
   * or keep the current data.
   */
  virtual void dataError() = 0;
  
  /**
   * Implements the Vatsim msg0 directive.
   * User will be notified about something by Vatsim servers. The dialog should
   * contain "do not show this message again" checkbox, which initial state is
   * unchecked.
   * 
   * \param message Vatsim message that follows the msg0 directive.
   */
  virtual void showVatsimMessage(const QString& message) = 0;
  
  /**
   * Shows airport details.
   * 
   * \param airport The Airport instance pointer.
   */
  virtual void showDetails(const Airport* airport) = 0;
  
  /**
   * Shows client details.
   * 
   * \param client The Client instance pointer.
   */
  virtual void showDetails(const Client* client) = 0;
  
  /**
   * Shows FIR details.
   * 
   * \param fir The FIR instance pointer.
   */
  virtual void showDetails(const Fir* fir) = 0;
  
  /**
   * Shows airport METAR.
   * 
   * \param metar The ICAO code of the airport.
   */
  virtual void showMetar(const QString& metar) = 0;
  
  /**
   * Shows dialog that lets user decide whether he wants to send anonymous
   * statistics or not. When user makes the decision, send a DecisionEvent
   * to the VatsinatorApplication singleton.
   */
  virtual void showStatsDialog() = 0;
  
  /**
   * Makes sure that the main window of the application is visible and top-level.
   * This function is called when user does some action that involves the map,
   * but it is possible that the map is not a top-level window.
   */
  virtual void ensureMainWindowIsActive() = 0;
  
protected:
  virtual bool notificationEvent(NotificationEvent* event);
  
};

#endif // USERINTERFACE_H
