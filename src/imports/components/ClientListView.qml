/*
 * ClientListView.qml
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
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import org.eu.vatsinator.Vatsim 1.0

/**
 * \ingroup Imports
 * 
 * \code{.qml}
 * import org.eu.vatsinator.Components
 * \endcode
 * 
 * The ClientListView represents a list of clients of the given \ref object.
 */
ListView {
    id: root
    
    /** type:ClientList The list object. */
    property var object
    
    /** type:function Returns an ATC line; the default value returns the real name. */
    property var atcSecondaryText: function(atc) {
        return atc.realName;
    }
    
    /** type:function Returns a flight line; the default value returns departure and destination airports' ICAO codes. */
    property var flightSecondaryText: function(flight) {
        if (flight.departure !== null && flight.destination !== null)
            return flight.departure.icao + " -> " + flight.destination.icao;
        else if (flight.departure !== null)
            return qsTr("from %1").arg(flight.departure.icao);
        else if (flight.destination !== null)
            return qsTr("to %1").arg(flight.destination.icao);
    }
    
    /**
     * The ATC was clicked.
     * \param atc The ATC that was clicked.
     */
    signal atcClicked(var atc)
    
    /**
     * The flight was clicked.
     * \param flight The flight that was clicked.
     */
    signal flightClicked(var flight)
    
    /**
     * Converts client type (pilot/ATC) to a string describing it.
     * \param type The type of the client ("ATC"/"PILOT").
     */
    function clientTypeToString(type)
    {
        if (type === "ATC")
            return qsTr("ATCs");
        else if (type === "PILOT")
            return qsTr("Flights");
        else
            return "";
    }
    
    model: ModelFactory.clients(object)
    
    // The delegate for each section header
    Component {
        id: sectionHeading
        
        Item {
            width: parent.width
            height: 48

            Label {
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.verticalCenter: parent.verticalCenter
                
                color: "#757575"
                font.pixelSize: 14
                font.weight: Font.Medium
                
                text: clientTypeToString(section)
            }
        }
    }
    
    anchors.fill: parent
    anchors.margins: 0
    
    delegate: TwoLineListItem {
        primaryText: callsign
        secondaryText: type == "ATC" ? atcSecondaryText(instance) : flightSecondaryText(instance)
        iconSource: type == "ATC" ? "qrc:/icons/maps/traffic_black.svg" : "qrc:/icons/maps/flight_black.svg"
        
        onPrimaryClicked: {
            if (type == "ATC")
                root.atcClicked(instance);
            else if (type== "PILOT")
                root.flightClicked(instance);
            else
                console.warn("Unknown client type: " + type);
        }
    }
    
    section.property: "type"
    section.criteria: ViewSection.FullString
    section.delegate: sectionHeading
}
