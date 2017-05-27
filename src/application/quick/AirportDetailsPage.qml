/*
 * AirportDetailsPage.qml
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
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import org.eu.vatsinator.Components 1.0
import org.eu.vatsinator.Vatsim 1.0

Page {
    id: root
    
    /// type:Vatsinator::Core::AirportObject* The airport
    property var airport: null
    
    signal atcClicked(var atc)
    signal flightClicked(var flight)
    
    function gMapsUrl(position, width)
    {
        var key = "AIzaSyCS3_JyszlVzsbweRQPd_dthosQbAF9XW0";
        var sourceWidth = width / 2;
        var sourceHeight = parseInt(sourceWidth / 16 * 9);
        var zoom = 13;
        
        return "https://maps.googleapis.com/maps/api/staticmap?"
            + "center=" + position.y + "," + position.x + "&"
            + "zoom=" + zoom + "&"
            + "size=" + sourceWidth + "x" + sourceHeight + "&"
            + "scale=2&"
            + "key=" + key;
    }
    
    function flightDescription(flight)
    {
        if (flight.departure === root.airport)
            return flight.destination.known ? qsTr("to %1").arg(flight.destination.representativeName()) : "";
        else if (flight.destination === root.airport)
            return flight.departure.known ? qsTr("from %1").arg(flight.departure.representativeName()) : "";
        else
            return flight.departure.icao + " -> " + flight.destination.icao
    }
    
    header: ToolBar {
        id: toolbar
        
        height: 24 + 56 + 48
        
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
                
                text: airport.representativeName()
            }
            
            TabBar {
                id: tabs
                
                Layout.columnSpan: 2
                Layout.fillWidth: true
                Layout.preferredHeight: 48
                
                Material.theme: Material.Dark
                Material.background: Material.primary
                Material.foreground: "white"
                Material.accent: "white"
                
                currentIndex: swipe.currentIndex
                
                TabButton {
                    height: 48
                    
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    
                    text: qsTr("INFO");
                }
                
                TabButton {
                    height: 48
                    
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    
                    text: qsTr("CLIENTS");
                }
                
                TabButton {
                    height: 48
                    
                    font.pixelSize: 14
                    font.weight: Font.Medium
                    
                    text: qsTr("NOTAM");
                }
            }
        }
    }
    
    footer: ToolBar {
        height: Screen.orientation === Qt.PortraitOrientation ? deviceInfo.navigationBarHeight : 0
    }
    
    SwipeView {
        id: swipe
        
        anchors.margins: 0
        anchors.fill: parent
        
        currentIndex: tabs.currentIndex
        
        Item {
            Flickable {
                anchors.fill: parent
                contentWidth: parent.width
                contentHeight: details.implicitHeight
                flickableDirection: Flickable.VerticalFlick
                
                GridLayout {
                    id: details
                    
                    anchors.fill: parent
                    anchors.margins: 0
                    
                    columns: 1
                    rowSpacing: 0
                    columnSpacing: 0
                    
                    Item { Layout.preferredHeight: 8 }
                    
                    Details {
                        name: qsTr("Name")
                        value: airport.name
                        iconSource: "qrc:/icons/action/label_black.svg"
                    }
                    
                    Details {
                        name: qsTr("City")
                        value: airport.city
                        iconSource: "qrc:/icons/social/location_city_black.svg"
                    }
                    
                    Details {
                        name: qsTr("Country")
                        value: airport.country
                        iconSource: "qrc:/icons/content/flag_black.svg"
                    }
                    
                    Details {
                        name: qsTr("Altitude")
                        value: qsTr("%1 feet").arg(airport.altitude)
                        iconSource: "qrc:/icons/image/filter_hdr_black.svg"
                    }
                    
                    Image {
                        id: airportVicinity
                        
                        Layout.preferredWidth: 360
                        Layout.preferredHeight: 240
                        fillMode: Image.PreserveAspectFit
                        
                        source: gMapsUrl(airport.position, 1080)
                        
                        BusyIndicator {
                            anchors.centerIn: parent
                            running: airportVicinity.status == Image.Loading
                        }
                    }
                }
            }
        }
        
        Item {
            ClientListView {
                anchors.fill: parent
                anchors.margins: 0
                
                object: airport
                flightSecondaryText: flightDescription
                
                onAtcClicked: root.atcClicked(atc)
                onFlightClicked: root.flightClicked(flight)
            }
        }
        
        Item {
            ListView {
                anchors.fill: parent
                model: ModelFactory.notams(airport)
                delegate: Label {
                    text: icao + " " + ident + " " + notam
                }
            }
        }
    }
}
