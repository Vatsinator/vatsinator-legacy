/*
 * MainMenu.qml
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

import QtQuick 2.4
import QtGraphicalEffects 1.0

Item {
    id: root
    
    signal clicked(url page)
    
    property alias color: container.color
    
    width: parent.width
    height: parent.height
    
    /* Menu container */
    Rectangle {
        id: container
        
        anchors.fill: parent
        color: palette.background()
        
        ListView {
            id: menuList
            anchors.fill: parent
            anchors.topMargin: 30
            
            model: MenuModel {}
            delegate: MenuListDelegate {
                onClicked: {
                    if (page) {
                        root.clicked(page)
                    }
                }
            }
        }
    }
}