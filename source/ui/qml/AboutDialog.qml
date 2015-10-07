/*
 * AboutDialog.qml
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
import QtQuick.Layouts 1.1

Rectangle {
    id: root
    width: parent.width
    height: parent.height
    
    color: palette.background()
    
    ColumnLayout {
        spacing: 10
        anchors.fill: parent
        
        Label {
            text: "Vatsinator"
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
        }
        
        Label {
            text: qsTr("A simple Vatsim monitor")
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
