/*
 * flightdetailswindow.h
 * Copyright (C) 2012  Michał Garapich <michal@garapich.pl>
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

#ifndef WIDGETS_FLIGHTDETAILSWINDOW_H
#define WIDGETS_FLIGHTDETAILSWINDOW_H

#include "core/airport.h"
#include "core/pilot.h"
#include "widgets/vtrwidgets_export.h"
#include <QtCore/QPointer>
#include <QtWidgets/QWidget>

namespace Ui { class FlightDetailsWindow; }

namespace Vatsinator { namespace Widgets {

/**
 * \ingroup Widgets
 * @{
 * 
 * The FlightDetailsWindow provides an easy and simple way to display all flight
 * details in a nice window.
 */
class VTRWIDGETS_EXPORT FlightDetailsWindow : public QWidget {
    Q_OBJECT
    
    /**
     * Keeps instance of the flight.
     */
    Q_PROPERTY(const Vatsinator::Core::Pilot* flight READ flight WRITE setFlight)
    
signals:
    /**
     * Emitted when user cliks the airport button.
     */
    void airportDetailsRequested(Core::Airport* airport);
    
    /**
     * Emitted when user wants to show the airport on the map.
     */
    void showOnMapRequested();

    /**
     * Emitted when user toggles the "Track this flight" checkbox.
     */
    void flightTrackingToggled(bool tracked);
    
public:
    /**
     * Creates a bew flight details window.
     */
    explicit FlightDetailsWindow(QWidget* parent = nullptr);
    
    /**
     * Destroys the FlightDetailsWindow instance.
     */
    virtual ~FlightDetailsWindow();
    
    const Core::Pilot* flight() const { return m_flight.data(); }
    void setFlight(const Core::Pilot* flight);
    
protected:
    /**
     * \copydoc QWidget::showEvent()
     */
    void showEvent(QShowEvent* event) override;
    
    /**
     * \copydoc QWidget::changeEvent()
     */
    void changeEvent(QEvent* event) override;
    
private:
    void flightUpdated();
    
private slots:
    void updateCallsign(QString callsign);
    void updateRealName(QString realName);
    void updateDeparture(Core::Airport* departure);
    void updateDestination(Core::Airport* destination);
    void updateStd(QTime std);
    void updateSta(QTime sta);
    void updateAtd(QTime atd);
    void updateEta();
    void updateSquawk(QString squawk);
    void updateGroundSpeed(int groundSpeed);
    void updateHeading(int heading);
    void updateAltitude(int altitude);
    void updateOnlineFrom(QDateTime onlineFrom);
    void updateFlightPhase(Core::Pilot::FlightPhase phase);
    void updateAircraft(QString aircraft);
    void updateFlightRules(Core::Pilot::FlightRules rules);
    void updateCruiseAltitude(QString cruiseAltitude);
    void updatePlannedTas(int plannedTas);
    void updateFlightPlan(Core::FlightPlan flightPlan);
    void updateRemarks(QString remarks);
    void updateAirline(const Core::Airline& airline);
    void airlineLogoUpdated();
    void handleAirportButton();
    
private:
    QScopedPointer<Ui::FlightDetailsWindow> ui;
    QPointer<const Core::Pilot> m_flight;
    
}; /** @} */

}} /* namespace Vatsinator::Widgets */

#endif // WIDGETS_FLIGHTDETAILSWINDOW_H
