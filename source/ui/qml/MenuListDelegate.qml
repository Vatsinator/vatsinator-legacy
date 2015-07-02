/*
 * MenuListDelegate.qml
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

import QtQuick 2.4

Rectangle {
    id: root
    
    signal clicked
    
    width: parent.width
    height: 48 * dp
    
    color: mouse.pressed ? "#3E3E3E" : container.color
    
    Row {
        anchors.fill: parent
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 16 * dp
        spacing: 16 * dp
        
        Image {
            source: icon
            anchors.verticalCenter: parent.verticalCenter
            sourceSize.width: 56 * dp
        }
    
        Text {
            anchors.verticalCenter: parent.verticalCenter
            color: "#E5E5E5"
            text: name
        }
    }
    
    MouseArea {
        id: mouse
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
