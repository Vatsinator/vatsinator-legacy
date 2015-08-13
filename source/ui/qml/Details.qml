/*
 * Details.qml
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

Rectangle {
    id: root
    
    property bool open: false
    
    function close()
    {
        open = false;
    }
    
    function show()
    {
        open = true;
    }
    
    y: parent.height
    
    Behavior on y {
        SmoothedAnimation {
            duration: 200
            
            onRunningChanged: {
                if (!running && !open)
                    root.destroy()
            }
        }
    }
    
    states: [
        State {
            name: "hidden"
            when: root.open == false
            PropertyChanges { target: root; y: parent.height }
        },
        State {
            name: "visible"
            when: root.open == true
            PropertyChanges { target: root; y: parent.height - height - (24 * dp) }
        }
    ]
}
