/*
 * AtcDetailsPage.qml
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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import org.eu.vatsinator.Components 1.0
import org.eu.vatsinator.Vatsim 1.0

Page {
    id: root
    
    /// type:Vatsinator::Core::Atc* The flight.
    property var atc: null
    
    signal airportClicked(var airport)
    
    header: ToolBar {
        id: toolbar
        
        height: 24 + 56
        
        GridLayout {
            columns: 2
            columnSpacing: 0
            rowSpacing: 0
            anchors.fill: parent
            
            Item {
                Layout.preferredHeight: 24
                Layout.columnSpan: 2
                Layout.fillWidth: true
            }
            
            Item {
                Layout.preferredHeight: 56
                Layout.preferredWidth: 56
            
                ToolButton {
                    anchors.centerIn: parent
                    height: 48
                    width: 48
                    
                    Image {
                        anchors.centerIn: parent
                        sourceSize.height: 24 * Screen.devicePixelRatio
                        sourceSize.width: 24 * Screen.devicePixelRatio
                        height: 24
                        width: 24
                        
                        source: "qrc:/icons/navigation/arrow_back_white.svg"
                    }
                    
                    onClicked: stackView.pop()
                }
            }
            
            Label {
                Layout.fillWidth: true
                
                font.pixelSize: 20
                font.weight: Font.Medium
                
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignVCenter
                Material.theme: Material.Dark
                
                text: atc.callsign
            }
        }
    }
    
    footer: ToolBar {
        height: Screen.orientation === Qt.PortraitOrientation ? deviceInfo.navigationBarHeight : 0
    }
}
