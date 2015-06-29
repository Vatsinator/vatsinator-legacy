/*
 * MainMenu.qml
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

import QtQuick 2.3
import QtGraphicalEffects 1.0

Item {
    id: root
    
    signal clicked(url page)
    
    property int swipe: 0
    property alias color: container.color
    
    width: parent.width
    height: parent.height
    x: width * -1
    
    function toggleState() {
        if (root.state == "shown")
            root.state = "hidden";
        else
            root.state = "shown";
    }
    
    /* Menu container shadow */
    RectangularGlow {
        anchors.fill: container
        glowRadius: 25.0
        color: "#262626"
        cornerRadius: 0
    }
    
    /* Menu container */
    Rectangle {
        id: container
        
        anchors.fill: parent
        anchors.rightMargin: parent.width * 0.1
        color: "#363636"
        
        ListView {
            id: menuList
            anchors.fill: parent
            anchors.topMargin: 30
            
            model: MenuModel {}
            delegate: MenuListDelegate {
                onClicked: {
                    if (page) {
                        root.state = "hidden"
                        root.clicked(page)
                    }
                }
            }
        }
    }
    
    SwipeHandler {
        anchors {
            left: container.right
            right: root.right
            bottom: root.bottom
            top: root.top
        }
        
        onSwipeEnded: root.toggleState()
        onSwipeContinues: root.swipe += diffX
    }
    
    onSwipeChanged: x = root.swipe - root.width
    
    Keys.onReleased: {
        if (event.key == Qt.Key_Back) {
            root.state = "hidden"
            event.accepted = true
        }
    }
    
    states: [
        State {
            name: "shown"
            PropertyChanges { target: root; swipe: root.width; focus: true }
        },
        State {
            name: "hidden"
            PropertyChanges { target: root; swipe: 0; focus: false }
        }
    ]
    
    transitions: [
        Transition {
            to: "*"
            PropertyAnimation { target: root; property: "swipe"; duration: 200; easing.type: Easing.OutQuad }
        }
    ]
}
