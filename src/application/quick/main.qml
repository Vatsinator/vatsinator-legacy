import QtQuick 2.6
import QtQml 2.2
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import org.eu.vatsinator.Components 1.0
import org.eu.vatsinator.Vatsim 1.0

ApplicationWindow {
    id: vatsinator
    
    title: "Vatsinator"
    visible: true

    width: 480
    height: 620
    
    StackView {
        id: stackView
        anchors.fill: parent
        
        initialItem: mapPage
        
        // Implements back key navigation
        focus: true
        Keys.onReleased: {
            if (event.key === Qt.Key_Back) {
                if (stackView.depth > 1) {
                    stackView.pop();
                    event.accepted = true;
                } else {
                    event.accepted = false;
                }
            }
        }
        
        Component {
            id: mapPage
            
            Item {
                MapControls {
                    id: controls
                    anchors.fill: parent
                }
                
                Map {
                    id: map
                    anchors.fill: parent
                    
                    serverTracker: VatsimServerTracker
                    modelMatcher: ModelMatcher {
                        resourceFile: ResourceFile { name: "data/model.json" }
                    }
                    controls: controls
                    
                    onFlightSelected: {
                        stackView.push(flightDetailsPage.createObject(stackView, { "flight": item.pilot }));
                    }
                    
                    onAirportSelected: {
                        stackView.push(airportDetailsPage.createObject(stackView, { "airport": item.airport }));
                    }
                    
                    onFirSelected: {
                        stackView.push(firDetailsPage.createObject(stackView, { "fir": item.fir }));
                    }
                }
            }
        }
        
        Component {
            id: flightDetailsPage
            FlightDetailsPage {
                onAirportClicked: stackView.push(airportDetailsPage.createObject(stackView, { "airport": airport }));
            }
        }
        
        Component {
            id: atcDetailsPage
            AtcDetailsPage {
                onAirportClicked: stackView.push(airportDetailsPage.createObject(stackView, { "airport": airport }));
            }
        }
        
        Component {
            id: airportDetailsPage
            AirportDetailsPage {
                onAtcClicked: stackView.push(atcDetailsPage.createObject(stackView, { "atc": atc }));
                onFlightClicked: stackView.push(flightDetailsPage.createObject(stackView, { "flight": flight }));
            }
        }
        
        Component {
            id: firDetailsPage
            FirDetailsPage {
                onAtcClicked: stackView.push(atcDetailsPage.createObject(stackView, { "atc": atc }));
                onFlightClicked: stackView.push(flightDetailsPage.createObject(stackView, { "flight": flight }));
            }
        }
    }
}
