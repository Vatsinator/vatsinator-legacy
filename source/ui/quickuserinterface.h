/*
 * quickuserinterface.h
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef QUICKUSERINTERFACE_H
#define QUICKUSERINTERFACE_H

#include <QQmlApplicationEngine>

#include "ui/userinterface.h"
#include "vatsinatorapplication.h"

/**
 * This is the mobile version of the user interface, based on QtQuick.
 */
class QuickUserInterface : public UserInterface {
    Q_OBJECT
    
public:
    explicit QuickUserInterface(QObject* parent = nullptr);
    
    /**
     * Gives direct access to the root Vatsinator window item.
     * Returns null if the qml was not loaded yet.
     */
    QObject* rootItem();
    
    /**
     * Finds an item by the given name in the QML tree, beginning
     * with _rootItem()_.
     */
    QObject* findObjectByName(const QString& name);
    
    /**
     * Returns "dp" units for the current device.
     */
    qreal dp() const;
    
    /**
     * \copydoc UserInterface::initialize()
     * 
     * The following properties are bound to the QML context:
     * * \c flights - the VatsimDataHandler's flights model;
     * * \c atcs - the VatsimDataHandler's atc model;
     * * \c android - \ref AndroidBridge instance.
     */
    void initialize() override;
    
    /**
     * Defines the minimum touch target size for the current device,
     * in pixels.
     * 
     * The value is always 48x48 dp.
     */
    inline QSize minimumTouchTarget() const
    {
        return QSize(48 * dp(), 48 * dp());
    }
    
public slots:
    /**
     * \copydoc UserInterface::fatal()
     */
    void fatal(const QString& message) override;
    
    /**
     * \copydoc UserInterface::warning()
     */
    void warning(const QString& message) override;
    
    /**
     * \copydoc UserInterface::showDetails(const Airport*)
     */
    void showDetails(const Airport* airport) override;
    
    /**
     * \copydoc UserInterface::showDetails(const Client*)
     */
    void showDetails(const Client* client) override;
    
    /**
     * \copydoc UserInterface::showDetails(const Fir*)
     */
    void showDetails(const Fir* fir) override;
    
    /**
     * \copydoc UserInterface::showMetar()
     */
    void showMetar(const QString& metar) override;
    
    /**
     * \copydoc UserInterface::showStatsDialog()
     */
    void showStatsDialog() override;
    
    /**
     * \copydoc UserInterface::ensureMainWindowIsActive()
     */
    void ensureMainWindowIsActive() override;

protected:
    /**
     * \copydoc UserInterface::vatsimEvent()
     */
    void vatsimEvent(VatsimEvent *event) override;
    
private:
    QQmlApplicationEngine __engine;
    
};

/**
 * You can use this function to access specific function of QuickUserInterface
 * implementation as well as check the current ui mode.
 */
inline QuickUserInterface* qui()
{
    return dynamic_cast<QuickUserInterface*>(vApp()->userInterface());
}

#endif // QUICKUSERINTERFACE_H
