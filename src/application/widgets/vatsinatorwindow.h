/*
 * vatsinatorwindow.h
 * Copyright (C) 2012 Michał Garapich <michal@garapich.pl>
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

#ifndef VATSINATORWINDOW_H
#define VATSINATORWINDOW_H

#include <core/airport.h>
#include <core/atc.h>
#include <core/pilot.h>
#include <core/servertracker.h>
#include <widgets/mapinfowidget.h>
#include <widgets/metarwindow.h>
#include <widgets/clientlistwindow.h>
#include <QMainWindow>

namespace Ui { class VatsinatorWindow; }
class SettingsWindow;

class VatsinatorWindow : public QMainWindow {
    Q_OBJECT
    
public:
    explicit VatsinatorWindow();
    
    virtual ~VatsinatorWindow();
    
protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void changeEvent(QEvent* event) override;
    
private slots:
    void showAirportDetails(const Vatsinator::Core::Airport* airport);
    void showFlightDetails(const Vatsinator::Core::Pilot* pilot);
    void showAtcDetails(const Vatsinator::Core::Atc* atc);
    void showFirDetails(const Vatsinator::Core::Fir* fir);
    void showClientDetails(const QString& callsign);
    void updateMapInfo();
    void showSettingsWindow();
    void setMapDrawerPlugin(const QVariant& name);
    void updateMapAddons(const QVariant& addons);
    void showMetarWindow();
    void showMetarWindow(const QString& icaoSearch);
    void showClientListWindow();

private:
    QScopedPointer<Ui::VatsinatorWindow> ui;
    Vatsinator::Core::ServerTracker* m_server;
    Vatsinator::Widgets::MapInfoWidget* m_mapInfo;
    Vatsinator::Widgets::MetarWindow* m_metars = nullptr;
    Vatsinator::Widgets::ClientListWindow* m_clients = nullptr;
    SettingsWindow* m_settings = nullptr;
};

#endif // VATSINATORWINDOW_H
