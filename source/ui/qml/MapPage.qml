/*
 * MapView.qml
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
import QtQuick.Dialogs 1.2
import org.eu.vatsinator.ui 1.0

/**
 * The MapView consists of the map itself, the menu drawer (the small
 * item in the bottom-left corner of the screen) and the menu that slides.
 */
Item {
    id: root
    objectName: "mapPage"
    
    signal itemTouched(var item)
    
    property var flightDetails: null /**< FlightDetails component */
    property var airportDetails: null /**< AirportDetails component */
    property var firDetails: null /**< FirDetails component */
    property var currentDetails: null /**< Currently shown details box */
    
    function showFlightDetails(flight)
    {
        var fd = root.flightDetails.createObject(root, {
            width: root.width - 56 * dp,
            height: 168 * dp,
            flight: flight
        });
        
        root.showDetails(fd);
    }
    
    function showAirportDetails(airport)
    {
        var ad = root.airportDetails.createObject(root, {
            width: root.width - 56 * dp,
            height: 168 * dp,
            airport: airport
        });
        
        root.showDetails(ad);
    }
    
    function showFirDetails(fir)
    {
        var fd = root.firDetails.createObject(root, {
            width: root.width - 56 * dp,
            height: 56 * dp,
            fir: fir
        });
        
        root.showDetails(fd);
    }
    
    function showDetails(details)
    {
        console.assert(details != null);
        if (root.currentDetails)
            root.currentDetails.close();
        
        details.show();
        root.currentDetails = details;
    }
    
    function hideCurrentDetails()
    {
        if (root.currentDetails) {
            root.currentDetails.close();
            root.currentDetails = null;
            map.selectedItem = undefined;
        }
    }
    
    function updateSelectedItem(item)
    {
        map.selectedItem = item;
    }
    
    width: parent.width
    height: parent.height
    
    Map {
        id: map
        anchors.fill: parent
        
        onCenterChanged: root.hideCurrentDetails()
        onZoomChanged: root.hideCurrentDetails()
    }
    
    /* This small icon in the lower-left corner that makes user aware of the menu */
    MenuDrawer {
        id: menuDrawer
        
        anchors {
            left: parent.left
            bottom: parent.bottom
            bottomMargin: parent.height / 20
        }
    }
    
    MapControls {
        anchors.fill: parent
        
        onZoomUpdated: map.updateZoom(zoom)
        onPositionUpdated: map.updatePosition(x, y)
        onClicked: {
            var item = map.itemUnderPosition(x, y);
            if (item != null) {
                root.itemTouched(item)
            } else {
                console.log("You did not click a thing!");
            }
        }
    }
    
    Component.onCompleted: {
        root.flightDetails = Qt.createComponent("FlightDetails.qml");
        root.airportDetails = Qt.createComponent("AirportDetails.qml");
        root.firDetails = Qt.createComponent("FirDetails.qml");
        root.itemTouched.connect(updateSelectedItem);
    }
}
