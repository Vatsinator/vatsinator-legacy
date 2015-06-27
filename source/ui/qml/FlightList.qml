/*
 * FlightList.qml
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

import QtQuick 2.4

Rectangle {
    width: parent.width
    height: parent.height
    
    color: "#2a313b"
    
    ListView {
        anchors.topMargin: 60
        anchors.bottomMargin: 60
        model: flights
        anchors.fill: parent
        delegate: FlightListDelegate {}
    }
    
    Image {
        source: "images/overlay_search.png"
        
        sourceSize.width: 150
        sourceSize.height: 150
        
        x: parent.width - 200;
        y: parent.height - 200;
    }
}
