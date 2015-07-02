/*
 * MenuModel.qml
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

import QtQuick 2.2

ListModel {
    ListElement {
        name: "Refresh"
        icon: "images/ic_menu_refresh.png"
    }
    
    ListElement {
        name: "Flights"
        icon: "images/ic_menu_flights.png"
        page: "FlightList.qml"
    }
    
    ListElement {
        name: "Controllers"
        icon: "images/ic_menu_atc.png"
        page: "AtcList.qml"
    }
    
    ListElement {
        name: "Airports"
        icon: "images/ic_menu_airports.png"
    }
    
    ListElement {
        name: "Settings"
        icon: "images/ic_menu_settings.png"
        page: "SettingsPage.qml"
    }
    
    ListElement {
        name: "About"
        icon: "images/ic_menu_about.png"
        page: "AboutDialog.qml"
    }
}
