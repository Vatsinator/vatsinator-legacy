/*
 * MapControls.qml
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

import QtQuick 2.5
import QtQuick.Window 2.2

MultiPointTouchArea {
    id: root
    
    signal zoomUpdated(real zoom)
    signal positionUpdated(int x, int y)
    signal clicked(int x, int y)
    
    maximumTouchPoints: 2
    
    onUpdated: {
        if (touchPoints.length == 1) {
            var tp = touchPoints[0];
            root.positionUpdated(tp.x - tp.previousX, tp.y - tp.previousY)
        } else if (touchPoints.length == 2) {
            var tp1 = touchPoints[0];
            var tp2 = touchPoints[1];
            
            /* Calculate zoom factor */
            var vec1 = Qt.vector2d(tp2.x - tp1.x, tp2.y - tp1.y);
            var vec0 = Qt.vector2d(tp2.previousX - tp1.previousX, tp2.previousY - tp1.previousY);
            
            var factor = vec1.length() / vec0.length();
            root.zoomUpdated(factor);
            
            /* Calculate center point */
            var x = (tp2.x + tp1.x) / 2;
            var y = (tp2.y + tp1.y) / 2;
            var previousX = (tp2.previousX + tp1.previousX) / 2;
            var previousY = (tp2.previousY + tp1.previousY) / 2;
            
            root.positionUpdated(x - previousX, y - previousY);
        }
    }
    
    onReleased: {
        if (touchPoints.length == 1) {
            var tp = touchPoints[0];
            if (tp.startX == tp.x && tp.startY == tp.y)
                root.clicked(tp.x, tp.y);
        }
    }
}