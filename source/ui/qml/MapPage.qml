/*
 * MapView.qml
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
import org.eu.vatsinator.ui 1.0

/**
 * The MapView consists of the map itself, the menu drawer (the small
 * item in the bottom-left corner of the screen) and the menu that slides.
 */
Item {
    id: root
    
    width: parent.width
    height: parent.height
    
    Map {
        id: map
        objectName: "map"
        anchors.fill: parent
    }
    
    /* This small icon in the lower-left corner that makes user aware of the menu */
    MenuDrawer {
        id: menuDrawer
        
        anchors {
            left: parent.left
            bottom: parent.bottom
            bottomMargin: parent.height / 20
        }
    }
    
    MapControls {
        anchors.fill: parent
        
        onZoomUpdated: map.updateZoom(zoom)
        onPositionUpdated: map.updatePosition(x, y)
        onClicked: console.log("pressed: ", x, ", ", y)
    }
}
