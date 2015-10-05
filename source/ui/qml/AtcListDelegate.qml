/*
 * AtcListDelegate.qml
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
import QtQuick.Controls 1.3

Rectangle {
    id: root
    
    signal clicked
    
    width: parent.width
    height: 72 * dp
    
    color: mouse.pressed ? palette.byHue(50) : palette.background()
    
    Item {
        height: lblCallsign.implicitHeight + lblDescription.implicitHeight
        anchors {
            left: parent.left
            leftMargin: 32 * dp
            right: parent.right
            rightMargin: 32 * dp
            verticalCenter: parent.verticalCenter
        }
        
        Label {
            id: lblCallsign
            anchors.left: parent.left
            font.weight: Font.Medium
            
            text: callsign
        }
        
        Label {
            id: lblRealName
            anchors.right: parent.right
            font.weight: Font.Medium
            
            text: realName
        }
        
        Label {
            id: lblDescription
            anchors.left: parent.left
            anchors.top: lblCallsign.bottom
            
            font.weight: Font.Medium
            color: palette.caption()
            
            text: description
        }
    }
    
    MouseArea {
        id: mouse
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
