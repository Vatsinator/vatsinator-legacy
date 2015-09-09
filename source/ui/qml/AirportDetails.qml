/*
 * AirportDetails.qml
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
import org.eu.vatsinator.Vatsim 1.0

Details {
    id: root
    
    property var airport
    
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
                    
                    text: airport ? airport.icao + "/" + airport.iata : ""
                }
                
                Text {
                    color: "#ffffff"
                    font.weight: Font.Medium
                    font.pixelSize: 12 * dp
                    anchors.horizontalCenter: parent.horizontalCenter
                    
                    text: airport ? airport.name + ", " + airport.city : ""
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
                
                Text {
                    font.weight: Font.Medium
                    font.pixelSize: 12 * dp
                    color: "#ffffff"
                    
                    text: metars.fetch(root.airport.icao).metar
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
            }
        }
    }
}
