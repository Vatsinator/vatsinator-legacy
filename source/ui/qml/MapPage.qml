/*
 * MapView.qml
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

import QtQuick 2.2
import org.eu.vatsinator.ui 1.0

/**
 * The MapView consists of the map itself, the menu drawer (the small
 * item in the bottom-left corner of the screen) and the menu that slides.
 */
Item {
    id: root
    
    signal clicked(url page)
    
    width: parent.width
    height: parent.height
    
    Map {
        id: map
        objectName: "map"
    }
    
    /* Semi-transparent overlay */
    Rectangle {
        anchors.fill: parent
        color: "#262626"
        opacity: (mainMenu.swipe / parent.width) * 0.8
    }
    
    Row {
        anchors.fill: parent
        
        SwipeHandler {
            objectName: "menuSwipeHandler"
            width: parent.width * 0.05
            height: parent.height
            
            onSwipeEnded: mainMenu.toggleState()
            onSwipeContinues: mainMenu.swipe += diffX
        }
        
        MapControls {
            width: parent.width * 0.95
            height: parent.height
            
            onZoomUpdated: map.updateZoom(zoom)
            onPositionUpdated: map.updatePosition(x, y)
        }
    }
    
    MenuDrawer {
        id: menuDrawer
        
        anchors {
            left: parent.left
            bottom: parent.bottom
            bottomMargin: parent.height / 20
        }
    }
    
    MainMenu {
        id: mainMenu
        objectName: "mainMenu"
        onClicked: root.clicked(page)
    }
}
