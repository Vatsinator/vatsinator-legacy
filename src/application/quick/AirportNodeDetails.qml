import QtQuick 2.6
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import org.eu.vatsinator.Vatsim 1.0

Label {
    property var airport
    property var time

    signal airportClicked(var airport)


    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
    font.pixelSize: 20
    color: "#212121"

    text: airport.icao

    MouseArea {
        anchors.fill: parent
        onClicked: root.airportClicked(airport)
    }
}
