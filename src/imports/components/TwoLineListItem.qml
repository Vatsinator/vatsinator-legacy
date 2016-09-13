/*
 * TwoLineListItem.qml
 * Copyright (C) 2016  Michał Garapich <michal@garapich.pl>
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

import QtQuick 2.6
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0

Item {
    id: root
    
    property string primaryText
    property string secondaryText
    property string iconSource
    
    signal primaryClicked
    signal secondaryClicked
    
    height: 60
    width: parent.width
    
    GridLayout {
        anchors.fill: parent
        
        columns: 2
        rows: 4
        
        columnSpacing: 0
        rowSpacing: 0
        
        Item {
            Layout.rowSpan: 4
            Layout.preferredWidth: 72
            Layout.fillHeight: true
            
            Image {
                id: icon
                
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.verticalCenter: parent.verticalCenter
                sourceSize.height: 24 * Screen.devicePixelRatio
                sourceSize.width: 24 * Screen.devicePixelRatio
                height: 24
                width: 24
                
                source: iconSource
            }
            
            ColorOverlay {
                anchors.fill: icon
                source: icon
                color: Material.accent
                cached: true
            }
        }
        
        Item { Layout.fillHeight: true }
        
        Label {
            Layout.fillWidth: true
            
            font.pixelSize: 13
            color: "#212121"
            
            text: primaryText
        }
        
        Label {
            Layout.fillWidth: true
            
            font.pixelSize: 13
            color: "#757575"
            
            text: secondaryText
        }
        
        Item { Layout.fillHeight: true }
    }
    
    MouseArea {
        anchors.fill: parent
        z: 1
        onClicked: root.primaryClicked()
    }
}