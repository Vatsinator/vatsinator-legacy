/*
    flightdetailswindow.cpp
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtWidgets>

#include "db/airline.h"
#include "db/airlinedatabase.h"
#include "db/airportdatabase.h"
#include "ui/widgetsuserinterface.h"
#include "ui/vatsinatorstyle.h"
#include "ui/map/maprenderer.h"
#include "ui/map/mapscene.h"
#include "ui/widgets/mapwidget.h"
#include "ui/windows/airportdetailswindow.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/widgetsuserinterface.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/pilot.h"
#include "netconfig.h"
#include "vatsinatorapplication.h"

#include "flightdetailswindow.h"

FlightDetailsWindow::FlightDetailsWindow(const Pilot* pilot, QWidget* parent) :
    QWidget(parent),
    __pilot(pilot)
{
    setupUi(this);
    
    VatsinatorStyle* style = qobject_cast<VatsinatorStyle*>(vApp()->style());
    
    /* Setup fonts fonts */
    QFont smaller = style->smallFont();
    StdLabel->setFont(smaller);
    StdUtcLabel->setFont(smaller);
    StaLabel->setFont(smaller);
    StaUtcLabel->setFont(smaller);
    AtdLabel->setFont(smaller);
    AtdUtcLabel->setFont(smaller);
    EtaLabel->setFont(smaller);
    EtaUtcLabel->setFont(smaller);
    
    OriginLabel->setFont(smaller);
    DestinationLabel->setFont(smaller);
    RouteLabel->setFont(smaller);
    RemarksLabel->setFont(smaller);
    
    CallsignLabel->setFont(style->h1Font());
    FromLabel->setFont(style->h2Font());
    ToLabel->setFont(style->h2Font());
    
    connect(__pilot, &Pilot::updated, this, &FlightDetailsWindow::__updateInfo);
    connect(__pilot, &Pilot::airportsUpdated, this, &FlightDetailsWindow::__updateAirports);
    connect(__pilot, &Pilot::invalid, this, &FlightDetailsWindow::close);
    
    connect(OriginButton, &AirportDetailsButton::clicked, wui(), &WidgetsUserInterface::showAirportDetails);
    connect(DestinationButton, &AirportDetailsButton::clicked, wui(), &WidgetsUserInterface::showAirportDetails);
    
    connect(ShowButton, &QPushButton::clicked, [this]() {
        wui()->mainWindow()->mapWidget()->renderer()->scene()->cancelFlightTracking();
        wui()->mainWindow()->mapWidget()->renderer()->scene()->moveTo(__pilot->position());
        close();
        vApp()->userInterface()->ensureMainWindowIsActive();
    });
    
    connect(TrackFlightBox, &QCheckBox::stateChanged, [this](int _state) {
        emit flightTrackingStateChanged(__pilot, _state);
    });
    
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), QDesktopWidget().screenGeometry(wui()->mainWindow())));
}

void
FlightDetailsWindow::showEvent(QShowEvent* event)
{
    Q_ASSERT(__pilot);
    
    if (wui()->mainWindow()->mapWidget()->renderer()->scene()->trackedFlight() == __pilot)
        TrackFlightBox->setCheckState(Qt::Checked);
    else
        TrackFlightBox->setCheckState(Qt::Unchecked);
    
    __updateInfo();
    __updateAirports();
    
    Q_UNUSED(event);
}

void
FlightDetailsWindow::__updateAirports()
{
    if (__pilot->origin()->isValid()) {
        FromCityLabel->setText(__pilot->origin()->city());
        
        QString text = QStringLiteral("%1 %2").arg(__pilot->origin()->icao(), __pilot->origin()->name());
        if (!__pilot->origin()->name().contains(__pilot->origin()->city()))
            text.append(" - ").append(__pilot->origin()->city());
        
        OriginButton->setText(text);
        OriginButton->setAirportPointer(__pilot->origin());
    } else {
        FromCityLabel->setText("");
        OriginButton->setText(__pilot->route().origin);
        OriginButton->setAirportPointer(nullptr);
    }
    
    if (__pilot->destination()->isValid()) {
        ToCityLabel->setText(__pilot->destination()->city());
        
        QString text = QStringLiteral("%1 %2").arg(__pilot->destination()->icao(), __pilot->destination()->name());
        if (!__pilot->destination()->name().contains(__pilot->destination()->city()))
            text.append(" - ").append(__pilot->destination()->city());
        
        DestinationButton->setText(text);
        DestinationButton->setAirportPointer(__pilot->destination());
    } else {
        ToCityLabel->setText("");
        DestinationButton->setText(__pilot->route().destination);
        DestinationButton->setAirportPointer(nullptr);
    }
}

void
FlightDetailsWindow::__updateInfo()
{
    setWindowTitle(tr("%1 - flight details").arg(__pilot->callsign()));
    CallsignLabel->setText(__pilot->callsign());
    
    Airline* myAirline = vApp()->airlineDatabase()->find(__pilot->callsign().left(3));
    
    if (myAirline) {
        QString tooltip = QString("%1 (%2)").arg(myAirline->name(), myAirline->country());
        AirlineLabel->setToolTip(tooltip);
        
        if (myAirline->logo().isNull()) {
            AirlineLabel->setText(myAirline->name());
            connect(myAirline, &Airline::logoFetched, this, &FlightDetailsWindow::__airlineUpdated);
            myAirline->requestLogo();
        } else
            AirlineLabel->setPixmap(QPixmap::fromImage(myAirline->logo()));
    } else {
        AirlineLabel->setPixmap(QPixmap());
        AirlineLabel->setText("");
        AirlineLabel->setToolTip("");
    }
    
    FromLabel->setText(__pilot->route().origin);
    ToLabel->setText(__pilot->route().destination);
    
    if (__pilot->origin()->isValid() && __pilot->destination()->isValid())
        FlightProgress->setValue(__pilot->progress());
    else
        FlightProgress->hide();
    
    PlannedDepartureTimeLabel->setText(__pilot->std().isValid() ? __pilot->std().toString("hh:mm") : "-");
    ActualDepartureTimeLabel->setText(__pilot->atd().isValid() ? __pilot->atd().toString("hh:mm") : "-");
    PlannedArrivalTimeLabel->setText(__pilot->sta().isValid() ? __pilot->sta().toString("hh:mm") : "-");
    EstimatedArrivalTimeLabel->setText(__pilot->eta().isValid() ? __pilot->eta().toString("hh:mm") : "-");
    
    PilotLabel->setValue(QString("%1 (%2)").arg(__pilot->realName(), QString::number(__pilot->pid())));
    AltitudeLabel->setValue(tr("%1 feet").arg(QString::number(__pilot->altitude())));
    GroundSpeedLabel->setValue(tr("%1 kts").arg(QString::number(__pilot->groundSpeed())));
    HeadingLabel->setValue(QString::number(__pilot->heading()));
    
    if (__pilot->phase() == Pilot::Airborne)
        FlightPhaseLabel->setValue(tr("airborne"));
    else if (__pilot->phase() == Pilot::Departing)
        FlightPhaseLabel->setValue(tr("departing"));
    else
        FlightPhaseLabel->setValue(tr("arrived"));
        
    ServerLabel->setValue(__pilot->server());
    TimeOnlineLabel->setValue(__pilot->onlineFrom().toString("dd MMM yyyy, hh:mm"));
    SquawkLabel->setValue(__pilot->squawk());
    AltimeterLabel->setValue(__pilot->pressure().mb % " / " % __pilot->pressure().ihg);
    
    FlightRulesLabel->setValue((__pilot->flightRules() == Pilot::Ifr) ? "IFR" : "VFR");
    AircraftLabel->setValue(__pilot->aircraft());
    TrueAirSpeedLabel->setValue(tr("%1 kts").arg(QString::number(__pilot->tas())));
    CruiseAltitudeLabel->setValue(__pilot->route().altitude);
    
    RouteField->setPlainText(__pilot->route().route);
    RemarksField->setPlainText(__pilot->remarks());
}

void
FlightDetailsWindow::__airlineUpdated()
{
    Airline* a = qobject_cast<Airline*>(sender());
    Q_ASSERT(a);
    
    const QImage& logo = a->logo();
    
    if (!logo.isNull())
        AirlineLabel->setPixmap(QPixmap::fromImage(logo));
}

