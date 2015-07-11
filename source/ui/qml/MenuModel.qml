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
        name: qsTr("Refresh")
        icon: "images/ic_refresh_black.png"
    }
    
    ListElement {
        name: qsTr("Flights")
        icon: "images/ic_flight_takeoff_black.png"
        page: "FlightList.qml"
    }
    
    ListElement {
        name: qsTr("Controllers")
        icon: "images/ic_traffic_black.png"
        page: "AtcList.qml"
    }
    
    ListElement {
        name: qsTr("Airports")
        icon: "images/ic_room_black.png"
    }
    
    ListElement {
        name: qsTr("Settings")
        icon: "images/ic_settings_black.png"
        page: "SettingsPage.qml"
    }
    
    ListElement {
        name: qsTr("About")
        icon: "images/ic_info_outline_black.png"
        page: "AboutDialog.qml"
    }
}
