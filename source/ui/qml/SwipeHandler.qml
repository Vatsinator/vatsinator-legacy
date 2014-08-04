/*
 * SwipeHandler.qml
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

MouseArea {
    id: root
    
    property int lastX: mouseX
    property bool gestureStarted: false
    
    signal swipeEnded()
    signal swipeContinues(var diffX)
    
    onPressed: {
        lastX = mouse.x
        mouse.accepted = true
    }
    
    onPositionChanged: {
        root.swipeContinues(mouse.x - lastX)
        lastX = mouse.x
    }
    
    onReleased: {
        root.swipeEnded()
    }
}
