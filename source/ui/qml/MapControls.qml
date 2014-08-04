/*
 * MapControls.qml
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

PinchArea {
  
    signal zoomUpdated(var zoom)
    signal positionUpdated(var x, var y)
  
    onPinchUpdated: {
        zoomUpdated(pinch.scale - pinch.previousScale)
    }
  
    MouseArea {
        property int lastX
        property int lastY
        
        anchors.fill: parent
        
        onPressed: {
            lastX = mouse.x;
            lastY = mouse.y;
            mouse.accepted = true;
        }
        
        onPositionChanged: {
            positionUpdated(mouse.x - lastX, mouse.y - lastY);
            lastX = mouse.x;
            lastY = mouse.y;
        }
    }
}
