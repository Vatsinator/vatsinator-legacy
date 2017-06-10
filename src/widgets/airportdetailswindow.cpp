/*
 * airportdetailswindow.cpp
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

#include "airportdetailswindow.h"
#include "ui_airportdetailswindow.h"
#include "widgetsprivate.h"
#include "core/metar.h"
#include "core/servertracker.h"
#include "misc/atcbookinglistmodel.h"
#include "misc/atcbookingprovider.h"
#include "misc/atclistmodel.h"
#include "misc/flightlistmodel.h"
#include "misc/notamlistmodel.h"
#include "misc/notamprovider.h"
#include "misc/pluginfinder.h"
#include "misc/roles.h"
#include <QtWidgets>

using namespace Vatsinator::Core;
using namespace Vatsinator::Misc;

namespace Vatsinator { namespace Widgets {

AirportDetailsWindow::AirportDetailsWindow(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::AirportDetailsWindow)
{
    ui->setupUi(this);
    
    connect(ui->inboundFlights, &QAbstractItemView::doubleClicked, this, &AirportDetailsWindow::showDetails);
    connect(ui->outboundFlights, &QAbstractItemView::doubleClicked, this, &AirportDetailsWindow::showDetails);
    connect(ui->atcs, &QAbstractItemView::doubleClicked, this, &AirportDetailsWindow::showDetails);
    connect(ui->show, &QPushButton::clicked, this, &AirportDetailsWindow::showOnMapRequested);
    
    QFont font = ui->iataicao->font();
    font.setPointSize(font.pointSize() + 2);
    ui->iataicao->setFont(font);
    
    ui->inboundFlights->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->outboundFlights->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->atcs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->bookings->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

AirportDetailsWindow::~AirportDetailsWindow() {}

void AirportDetailsWindow::setAirport(const Airport* airport)
{
    if (m_airport)
        disconnect(m_airport);
    
    m_airport = airport;
    
    setWindowTitle(tr("%1 - airport details").arg(airport->icao()));
    
    if (airport->iata().isEmpty())
        ui->iataicao->setText(airport->icao());
    else
        ui->iataicao->setText(QStringLiteral("%1 / %2").arg(airport->icao(), airport->iata()));
    
    ui->name->setText(QStringLiteral("%1, %2").arg(airport->name(), airport->city()));
    
    if (ui->inboundFlights->model())
        ui->inboundFlights->model()->deleteLater();
    
    ui->inboundFlights->setModel(FlightListModel::inbound(airport, this));
    ui->inboundFlights->horizontalHeader()->hideSection(3);
    
    if (ui->outboundFlights->model())
        ui->outboundFlights->model()->deleteLater();
    
    ui->outboundFlights->setModel(FlightListModel::outbound(airport, this));
    ui->outboundFlights->horizontalHeader()->hideSection(2);
    
    if (ui->atcs->model())
        ui->atcs->model()->deleteLater();
    
    ui->atcs->setModel(AtcListModel::staff(airport, this));
    
    ui->name_2->setText(airport->name());
    ui->city->setText(airport->city());
    ui->country->setText(airport->country());
    ui->altitude->setText(tr("%n foot(s)", "", airport->altitude()));
    
    updateMetar();
    updateNotams();
    updateBookings();
    
//    if (!m_connection)
//        m_connection = connect(airport->server()->metarManager(), &MetarManager::updated, this, &AirportDetailsWindow::updateMetar);
}

void AirportDetailsWindow::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    fixupGeometry(this);
}

void AirportDetailsWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        ui->altitude->setText(tr("%n foot(s)", "", m_airport->altitude()));
        setWindowTitle(tr("%1 - airport details").arg(m_airport->icao()));
        
        event->accept();
    } else {
        event->ignore();
    }
}

void AirportDetailsWindow::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
    updateMetar();
}

void AirportDetailsWindow::showDetails(const QModelIndex& index)
{
    QString callsign = index.data(Misc::CallsignRole).toString();
    Q_ASSERT(!callsign.isEmpty());
    emit clientDetailsRequested(callsign);
}

void AirportDetailsWindow::updateMetar()
{
    if (!airport())
        return;
    
//    Metar m = airport()->server()->metarManager()->find(airport()->icao());
//    QFontMetrics metrics(ui->metar->font());
//    int width = ui->metar->width() - 2;
//    QString clipped = metrics.elidedText(m.metar(), Qt::ElideRight, width);
//    ui->metar->setText(clipped);
//    ui->metar->setToolTip(m.metar());
}

void AirportDetailsWindow::updateNotams()
{
//    NotamListModel* m = nullptr;
    
//    if (NotamReply* nr = qobject_cast<NotamReply*>(sender())) {
//        m = new NotamListModel(nr->notams(), this);
//    } else {
//        auto plugins = PluginFinder::pluginsForIid(qobject_interface_iid<NotamProvider*>());
//        if (plugins.length() > 0) {
//            NotamProvider* p = qobject_cast<NotamProvider*>(PluginFinder::plugin(plugins.first()));
//            Q_ASSERT(p);
            
//            const NotamReply* nr = p->fetchNotams(m_airport->icao());
//            if (nr->isFinished())
//                m = new NotamListModel(nr->notams(), this);
//            else
//                connect(nr, &NotamReply::finished, this, &AirportDetailsWindow::updateNotams);
//        } else {
//            ui->notamTab->setEnabled(false);
//        }
//    }
    
//    if (ui->notams->model())
//        ui->notams->model()->deleteLater();
    
//    ui->notams->setModel(m);
}

void AirportDetailsWindow::updateBookings()
{
//    AtcBookingListModel* m = nullptr;
    
//    if (AtcBookingReply* br = qobject_cast<AtcBookingReply*>(sender())) {
//        m = new AtcBookingListModel(br->bookings(), this);
//    } else {
//        auto plugins = PluginFinder::pluginsForIid(qobject_interface_iid<AtcBookingProvider*>());
//        if (plugins.length() > 0) {
//            AtcBookingProvider* p = qobject_cast<AtcBookingProvider*>(PluginFinder::plugin(plugins.first()));
//            Q_ASSERT(p);
            
//            const AtcBookingReply* r = p->fetchBookings(m_airport->icao());
//            if (r->isFinished())
//                m = new AtcBookingListModel(r->bookings(), this);
//            else
//                connect(r, &AtcBookingReply::finished, this, &AirportDetailsWindow::updateBookings);
//        } else {
//            ui->bookingTab->setEnabled(false);
//        }
//    }
    
//    if (ui->bookings->model())
//        ui->bookings->model()->deleteLater();
    
//    ui->bookings->setModel(m);
}

}} /* namespace Vatsinator::Widgets */
