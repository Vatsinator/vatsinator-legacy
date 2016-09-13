/*
 * Detail.qml
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
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0

Item {
    id: root
    
    property string name
    property string value
    property string iconSource: ""
    
    implicitHeight: 72
    implicitWidth: Math.max(labelName.implicitWidth, labelValue.implicitWidth) + 32
    
    GridLayout {
        anchors.fill: parent
        columnSpacing: 0
        rowSpacing: 0
        columns: 2
        rows: 2
        
        Item {
            Layout.rowSpan: 2
            Layout.fillHeight: true
            Layout.preferredWidth: 72
            opacity: iconSource == "" ? 0.0 : 1.0
            
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
        
        Label {
            id: labelValue
            
            Layout.alignment: Qt.AlignBottom | Qt.AlignLeft
            font.pixelSize: 16
            color: "#212121"
            
            text: value
        }
        
        Label {
            id: labelName
            
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            font.pixelSize: 13
            color: "#757575"
            
            text: name
        }
    }
}
