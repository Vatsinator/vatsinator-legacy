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
#include "events/vatsimevent.h"

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
     * Creates new instance of either of the implementations.
     */
    static UserInterface* instantiate(QObject* parent);
    
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
    /**
     * Custom event handling.
     */
    void customEvent(QEvent* event) override;
    
    /**
     * Handles Vatsim event.
     */
    virtual void vatsimEvent(VatsimEvent* event) = 0;
    
    /**
     * Handles notification event.
     */
    virtual void notificationEvent(NotificationEvent* event);
    
};

#endif // USERINTERFACE_H
