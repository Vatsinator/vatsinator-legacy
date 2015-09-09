/*
 * MenuDrawer.qml
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

import QtQuick 2.5
import QtGraphicalEffects 1.0

/* Menu drawer */
Item {
    id: root
    
    height: drawer.height + (2 * drawerShadow.glowRadius)
    width: 24 * dp + drawer.anchors.leftMargin + (2 * drawerShadow.glowRadius)
    
    RectangularGlow {
        id: drawerShadow
        
        anchors.fill: parent
        glowRadius: 1.0
        color: "#393939"
        cornerRadius: drawer.radius + glowRadius
        anchors.leftMargin: drawer.anchors.leftMargin
    }
    
    Rectangle {
        id: drawer
        
        color: "white"
        border.width: 0
        radius: 10
        height: 48 * dp
        width: 24 * dp
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: -4 * dp
        
        Image {
            width: 18 * dp
            height: 18 * dp
            fillMode: Image.Stretch
            source: "images/ic_drawer.png"
            anchors.centerIn: parent
        }
    }
}
