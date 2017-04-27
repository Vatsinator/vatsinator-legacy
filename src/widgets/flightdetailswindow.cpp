/*
 * flightdetailswindow.cpp
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

#include "flightdetailswindow.h"
#include "ui_flightdetailswindow.h"
#include "widgetsprivate.h"
#include "core/pluginfinder.h"
#include "misc/airlinelogoprovider.h"
#include "misc/airlinelogoreply.h"

using namespace Vatsinator::Core;
using namespace Vatsinator::Misc;

namespace Vatsinator { namespace Widgets {

constexpr auto AirportObjectKey = "airportObjectPtr";

FlightDetailsWindow::FlightDetailsWindow(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::FlightDetailsWindow)
{
    ui->setupUi(this);
    
    connect(ui->departureAirport, &QPushButton::clicked, this, &FlightDetailsWindow::handleAirportButton);
    connect(ui->destinationAirport, &QPushButton::clicked, this, &FlightDetailsWindow::handleAirportButton);
    connect(ui->show, &QPushButton::clicked, this, &FlightDetailsWindow::showOnMapRequested);
    connect(ui->trackFlight, &QCheckBox::toggled, this, &FlightDetailsWindow::flightTrackingToggled);
    
    QFont font = ui->callsign->font();
    font.setPointSize(font.pointSize() + 2);
    ui->callsign->setFont(font);
}

FlightDetailsWindow::~FlightDetailsWindow() {}

void FlightDetailsWindow::setFlight(const Pilot* flight)
{
    if (!m_flight.isNull())
        disconnect(m_flight.data());
    
    m_flight = QPointer<const Pilot>(flight);
    flightUpdated();
}

void FlightDetailsWindow::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    fixupGeometry(this);
}

void FlightDetailsWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        updateGroundSpeed(m_flight->groundSpeed());
        updateAltitude(m_flight->altitude());
        updateFlightPhase(m_flight->flightPhase());
        
        event->accept();
    } else {
        event->ignore();
    }
}

void FlightDetailsWindow::flightUpdated()
{
    updateCallsign(m_flight->callsign());
    connect(m_flight.data(), &Pilot::callsignChanged, this, &FlightDetailsWindow::updateCallsign);
    
    updateRealName(m_flight->realName());
    connect(m_flight.data(), &Pilot::realNameChanged, this, &FlightDetailsWindow::updateRealName);
    
    updateDeparture(m_flight->departure());
    connect(m_flight.data(), &Pilot::departureChanged, this, &FlightDetailsWindow::updateDeparture);
    
    updateDestination(m_flight->destination());
    connect(m_flight.data(), &Pilot::destinationChanged, this, &FlightDetailsWindow::updateDestination);
    
    updateStd(m_flight->std());
    connect(m_flight.data(), &Pilot::stdChanged, this, &FlightDetailsWindow::updateStd);
    
    updateSta(m_flight->sta());
    connect(m_flight.data(), &Pilot::staChanged, this, &FlightDetailsWindow::updateSta);
    
    updateAtd(m_flight->atd());
    connect(m_flight.data(), &Pilot::atdChanged, this, &FlightDetailsWindow::updateAtd);
    
    updateEta();
    connect(m_flight.data(), &Pilot::etaChanged, this, &FlightDetailsWindow::updateEta);
    
    updateSquawk(m_flight->squawk());
    connect(m_flight.data(), &Pilot::squawkChanged, this, &FlightDetailsWindow::updateSquawk);
    
    updateGroundSpeed(m_flight->groundSpeed());
    connect(m_flight.data(), &Pilot::groundSpeedChanged, this, &FlightDetailsWindow::updateGroundSpeed);
    
    updateHeading(m_flight->heading());
    connect(m_flight.data(), &Pilot::headingChanged, this, &FlightDetailsWindow::updateHeading);
    
    updateAltitude(m_flight->altitude());
    connect(m_flight.data(), &Pilot::altitudeChanged, this, &FlightDetailsWindow::updateAltitude);
    
    updateOnlineFrom(m_flight->onlineFrom());
    connect(m_flight.data(), &Pilot::onlineFromChanged, this, &FlightDetailsWindow::updateOnlineFrom);
    
    updateFlightPhase(m_flight->flightPhase());
    connect(m_flight.data(), &Pilot::flightPhaseChanged, this, &FlightDetailsWindow::updateFlightPhase);
    
    updateCruiseAltitude(m_flight->cruiseAltitude());
    connect(m_flight.data(), &Pilot::cruiseAltitudeChanged, this, &FlightDetailsWindow::updateCruiseAltitude);
    
    updatePlannedTas(m_flight->plannedTas());
    connect(m_flight.data(), &Pilot::plannedTasChanged, this, &FlightDetailsWindow::updatePlannedTas);
    
    updateAircraft(m_flight->aircraft());
    connect(m_flight.data(), &Pilot::aircraftChanged, this, &FlightDetailsWindow::updateAircraft);
    
    updateFlightRules(m_flight->flightRules());
    connect(m_flight.data(), &Pilot::flightRulesChanged, this, &FlightDetailsWindow::updateFlightRules);
    
    updateFlightPlan(m_flight->flightPlan());
    connect(m_flight.data(), &Pilot::flightPlanChanged, this, &FlightDetailsWindow::updateFlightPlan);
    
    updateRemarks(m_flight->remarks());
    connect(m_flight.data(), &Pilot::remarksChanged, this, &FlightDetailsWindow::updateRemarks);
    
    updateAirline(m_flight->airline());
    connect(m_flight.data(), &Pilot::airlineChanged, this, &FlightDetailsWindow::updateAirline);
    
    ui->progress->setFlight(m_flight);
}

void FlightDetailsWindow::updateCallsign(QString callsign)
{
    ui->callsign->setText(callsign);
    setWindowTitle(callsign);
}

void FlightDetailsWindow::updateRealName(QString realName)
{
    ui->realName->setText(QStringLiteral("%1 (%2)").arg(realName, QString::number(m_flight->pid())));
}

void FlightDetailsWindow::updateDeparture(AirportObject* departure)
{
    if (departure) {
        ui->departure->setText(departure->icao());
        ui->departureCity->setText(departure->city());

        if (departure->isKnownAirport()) {
            QString text = QStringLiteral("%1 %2").arg(departure->icao(), departure->name());
            if (!departure->name().contains(departure->city()))
                text.append(" - ").append(departure->city());

            ui->departureAirport->setText(text);
        } else {
            ui->departureAirport->setText(departure->icao());
        }

        ui->departureAirport->setProperty(AirportObjectKey, QVariant::fromValue(departure));
        ui->departureAirport->setEnabled(departure->isKnownAirport());
    } else {
        ui->departure->setText(QString());
        ui->departureCity->setText(QString());
        ui->departureAirport->setText(QString());
        ui->departureAirport->setProperty(AirportObjectKey, QVariant::fromValue<AirportObject*>(nullptr));
        ui->departureAirport->setEnabled(false);
    }
}

void FlightDetailsWindow::updateDestination(AirportObject* destination)
{
    if (destination) {
        ui->destination->setText(destination->icao());
        ui->destinationCity->setText(destination->city());

        if (destination->isKnownAirport()) {
            QString text = QStringLiteral("%1 %2").arg(destination->icao(), destination->name());
            if (!destination->name().contains(destination->city()))
                text.append(" - ").append(destination->city());

            ui->destinationAirport->setText(text);
        } else {
            ui->destinationAirport->setText(destination->icao());
        }

        ui->destinationAirport->setProperty(AirportObjectKey, QVariant::fromValue(destination));
        ui->destinationAirport->setEnabled(destination->isKnownAirport());
    } else {
        ui->destination->setText(QString());
        ui->destinationCity->setText(QString());
        ui->destinationAirport->setText(QString());
        ui->destinationAirport->setProperty(AirportObjectKey, QVariant::fromValue<AirportObject*>(nullptr));
        ui->destinationAirport->setEnabled(false);
    }
}

void FlightDetailsWindow::updateStd(QTime std)
{
    static const QString tpl = QStringLiteral("<small>STD</small> <strong>%1</strong> <small>UTC</small>");
    ui->std->setText(tpl.arg(std.isValid() ? std.toString("hh:mm") : QStringLiteral("-")));
}

void FlightDetailsWindow::updateSta(QTime sta)
{
    static const QString tpl = QStringLiteral("<small>STA</small> <strong>%1</strong> <small>UTC</small>");
    ui->sta->setText(tpl.arg(sta.isValid() ? sta.toString("hh:mm") : QStringLiteral("-")));
}

void FlightDetailsWindow::updateAtd(QTime atd)
{
    static const QString tpl = QStringLiteral("<small>ATD</small> <strong>%1</strong> <small>UTC</small>");
    ui->atd->setText(tpl.arg(atd.isValid() ? atd.toString("hh:mm") : QStringLiteral("-")));
}

void FlightDetailsWindow::updateEta()
{
    static const QString tpl = QStringLiteral("<small>ETA</small> <strong>%1</strong> <small>UTC</small>");
    ui->eta->setText(tpl.arg(m_flight->eta().isValid() ? m_flight->eta().toString("hh:mm") : QStringLiteral("-")));
}

void FlightDetailsWindow::updateSquawk(QString squawk)
{
    ui->squawk->setText(squawk);
}

void FlightDetailsWindow::updateGroundSpeed(int groundSpeed)
{
    ui->groundSpeed->setText(tr("%1 knots").arg(QString::number(groundSpeed)));
}

void FlightDetailsWindow::updateHeading(int heading)
{
    ui->heading->setText(QString::number(heading));
}

void FlightDetailsWindow::updateAltitude(int altitude)
{
    ui->altitude->setText(tr("%1 feet").arg(QString::number(altitude)));
}

void FlightDetailsWindow::updateOnlineFrom(QDateTime onlineFrom)
{
    ui->onlineFrom->setText(onlineFrom.toString("dd MM yyyy, hh:mm"));
}

void FlightDetailsWindow::updateFlightPhase(Pilot::FlightPhase phase)
{
    switch (phase) {
        case Pilot::Departing:
            ui->flightPhase->setText(tr("departing"));
            break;
            
        case Pilot::Airborne:
            ui->flightPhase->setText(tr("airborne"));
            break;
            
        case Pilot::Arrived:
            ui->flightPhase->setText(tr("arrived"));
            break;
    }
}

void FlightDetailsWindow::updateAircraft(QString aircraft)
{
    ui->aircraft->setText(aircraft);
}

void FlightDetailsWindow::updateFlightRules(Pilot::FlightRules rules)
{
    ui->flightRules->setText(rules == Pilot::Ifr ? "IFR" : "VFR");
}

void FlightDetailsWindow::updateCruiseAltitude(QString cruiseAltitude)
{
    ui->cruiseAltitude->setText(cruiseAltitude);
}

void FlightDetailsWindow::updatePlannedTas(int plannedTas)
{
    ui->tas->setText(plannedTas > 0 ? tr("%1 knots").arg(QString::number(plannedTas)) : QString());
}

void FlightDetailsWindow::updateFlightPlan(FlightPlan flightPlan)
{
    ui->route->setText(flightPlan.route());
}

void FlightDetailsWindow::updateRemarks(QString remarks)
{
    ui->remarks->setText(remarks);
}

void FlightDetailsWindow::updateAirline(const Airline& airline)
{
    auto plugins = PluginFinder::pluginsForIid(qobject_interface_iid<AirlineLogoProvider*>());
    if (plugins.length() > 0) {
        AirlineLogoProvider* p = qobject_cast<AirlineLogoProvider*>(PluginFinder::plugin(plugins.first()));
        Q_ASSERT(p);
        
        const AirlineLogoReply* reply = p->fetchLogo(airline, ui->airline->size());
        if (reply->isFinished()) {
            ui->airline->setPixmap(reply->logo());
            ui->airline->setToolTip(airline.name());
        } else {
            ui->airline->setText(airline.name());
            connect(reply, &AirlineLogoReply::finished, this, &FlightDetailsWindow::airlineLogoUpdated);
        }
    } else {
        if (airline.name().isEmpty()) {
            ui->airline->setVisible(false);
        } else {
            ui->airline->setText(airline.name());
        }
    }
}

void FlightDetailsWindow::airlineLogoUpdated()
{
    const AirlineLogoReply* reply = qobject_cast<const AirlineLogoReply*>(sender());
    Q_ASSERT(reply);
    
    if (!reply->logo().isNull()) {
        ui->airline->setPixmap(reply->logo());
        ui->airline->setToolTip(reply->airline().name());
        
    }
}

void FlightDetailsWindow::handleAirportButton()
{
    QPushButton* pb = qobject_cast<QPushButton*>(sender());
    Q_ASSERT(pb);
    QVariant value = pb->property(AirportObjectKey);
    if (value.isValid()) {
        AirportObject* ap = value.value<AirportObject*>();
        Q_ASSERT(ap);
        emit airportDetailsRequested(ap);
    }
}

}} /* Vatsinator::Widgets */
