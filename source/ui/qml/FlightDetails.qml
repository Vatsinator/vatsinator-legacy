/*
 * FlightDetails.qml
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

Details {
    id: root
    
    property var flight
    
    anchors.horizontalCenter: parent.horizontalCenter
    color: palette.byHue(700)
    radius: 10 * dp
    
    Column {
        anchors.fill: parent
        
        Item {
            width: parent.width
            height: root.height / 3
            
            Column {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
            
                Text {
                    color: "#ffffff"
                    font.weight: Font.Bold
                    font.pixelSize: 24 * dp
                    anchors.horizontalCenter: parent.horizontalCenter
                    
                    text: root.flight ? root.flight.callsign : ""
                }
                
                Text {
                    color: "#ffffff"
                    font.weight: Font.Medium
                    font.pixelSize: 12 * dp
                    anchors.horizontalCenter: parent.horizontalCenter
                    
                    text: root.flight ? root.flight.realName : ""
                }
            
            }
        }
        
        Rectangle {
            width: parent.width
            height: root.height / 3
            
            gradient: Gradient { /* Nicely shaded rectangle */
                GradientStop { position: 0.0; color: Qt.darker(palette.byHue(900), 1.2) }
                GradientStop { position: 0.03; color: palette.byHue(900) }
                GradientStop { position: 0.97; color: palette.byHue(900) }
                GradientStop { position: 1.0; color: Qt.darker(palette.byHue(900), 1.2) }
            }
            
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                spacing: 30
                
                Column {
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.weight: Font.Medium
                        font.pixelSize: 24 * dp
                        color: "#ffffff"
                        
                        text: root.flight ? root.flight.origin.icao : ""
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: 12 * dp
                        color: "#ffffff"
                        
                        text: root.flight ? root.flight.origin.city : ""
                    }
                }
                
                Text {
                    font.weight: Font.Medium
                    font.pixelSize: 24 * dp
                    color: "#ffffff"
                    
                    text: "\u27a4"
                }
                
                Column {
                    Text {    
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.weight: Font.Medium
                        font.pixelSize: 24 * dp
                        color: "#ffffff"
                        
                        text: root.flight ? root.flight.destination.icao : ""
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        font.pixelSize: 12 * dp
                        color: "#ffffff"
                        
                        text: root.flight ? root.flight.destination.city : ""
                    }
                }
            }
        }
        
        Item {
            width: parent.width
            height: root.height / 3
            
            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                spacing: 20
                
                Column {
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: "#ffffff"
                        font.pixelSize: 10 * dp
                        
                        //: Aircraft model
                        text: qsTr("Aircraft")
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: "#ffffff"
                        font.pixelSize: 16 * dp
                        
                        text: root.flight ? root.flight.aircraft : ""
                    }
                }
                
                Column {
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: "#ffffff"
                        font.pixelSize: 10 * dp
                        
                        text: qsTr("Altitude")
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: "#ffffff"
                        font.pixelSize: 16 * dp
                        
                        text: root.flight ? root.flight.altitude + " ft" : ""
                    }
                }
                
                Column {
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: "#ffffff"
                        font.pixelSize: 10 * dp
                        
                        text: qsTr("Ground speed")
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: "#ffffff"
                        font.pixelSize: 16 * dp
                        
                        text: root.flight ? root.flight.groundSpeed + " kts" : ""
                    }
                }
                
                Column {
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: "#ffffff"
                        font.pixelSize: 10 * dp
                        
                        text: qsTr("Squawk")
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        color: "#ffffff"
                        font.pixelSize: 16 * dp
                        
                        text: root.flight ? root.flight.squawk : ""
                    }
                }
            }
        }
    }   
}
