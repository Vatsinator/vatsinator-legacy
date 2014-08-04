/*
 * quickuserinterface.h
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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
    virtual ~QuickUserInterface();
    
    void initialize() override;
    
public slots:
    void fatal(const QString& message) override;
    void warning(const QString& message) override;
    void statusError() override;
    void dataError() override;
    void showVatsimMessage(const QString& message) override;
    void showDetails(const Airport* airport) override;
    void showDetails(const Client* client) override;
    void showDetails(const Fir* fir) override;
    void showMetar(const QString& metar) override;
    void showStatsDialog() override;
    void ensureMainWindowIsActive() override;
    
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
