/*
 * firdetailswindow.cpp
 * Copyright (C) 2016 Michał Garapich <michal@garapich.pl>
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

#include "firdetailswindow.h"
#include "ui_firdetailswindow.h"
#include "widgetsprivate.h"
#include "core/pluginfinder.h"
#include "misc/atcbookinglistmodel.h"
#include "misc/atcbookingprovider.h"
#include "misc/atclistmodel.h"
#include "misc/flightlistmodel.h"
#include "misc/notamlistmodel.h"
#include "misc/notamprovider.h"
#include "misc/roles.h"
#include <QtWidgets>

using namespace Vatsinator::Core;
using namespace Vatsinator::Misc;

namespace Vatsinator { namespace Widgets {

FirDetailsWindow::FirDetailsWindow(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::FirDetailsWindow)
{
    ui->setupUi(this);
    
    connect(ui->flights, &QAbstractItemView::doubleClicked, this, &FirDetailsWindow::showDetails);
    connect(ui->atc, &QAbstractItemView::doubleClicked, this, &FirDetailsWindow::showDetails);
    
    QFont font = ui->icao->font();
    font.setPointSize(font.pointSize() + 2);
    ui->icao->setFont(font);
    
    ui->flights->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->atc->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->bookings->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

FirDetailsWindow::~FirDetailsWindow() {}

void FirDetailsWindow::setFir(const Core::FirObject* fir)
{
    if (m_fir)
        disconnect(m_fir);
    
    m_fir = fir;
    
    QString firartcc = fir->country() == "USA" ? "ARTCC" : "FIR";
    
    //: %1 is ICAO code, %2 is "FIR" or "ARTCC"
    setWindowTitle(tr("%1 - %2 details").arg(fir->icao(), firartcc));
    
    ui->icao->setText(fir->icao());
    ui->name->setText(fir->name());
    
    if (ui->flights->model())
        ui->flights->model()->deleteLater();
    
    ui->flights->setModel(FlightListModel::around(fir, this));
    
    if (ui->atc->model())
        ui->atc->model()->deleteLater();
    
    ui->atc->setModel(AtcListModel::staff(fir, this));
    
    updateNotams();
    updateBookings();
}

void FirDetailsWindow::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    fixupGeometry(this);
}

void FirDetailsWindow::showDetails(const QModelIndex& index)
{
    QString callsign = index.data(Misc::CallsignRole).toString();
    Q_ASSERT(!callsign.isEmpty());
    emit clientDetailsRequested(callsign);
}

void FirDetailsWindow::updateNotams()
{
    NotamListModel* m = nullptr;
    
    if (NotamReply* nr = qobject_cast<NotamReply*>(sender())) {
        m = new NotamListModel(nr->notams(), this);
    } else {
        auto plugins = PluginFinder::pluginsForIid(qobject_interface_iid<NotamProvider*>());
        if (plugins.length() > 0) {
            NotamProvider* p = qobject_cast<NotamProvider*>(plugins.first());
            Q_ASSERT(p);
            
            const NotamReply* nr = p->fetchNotams(m_fir->icao());
            if (nr->isFinished()) {
                m = new NotamListModel(nr->notams(), this);
            } else {
                connect(nr, &NotamReply::finished, this, &FirDetailsWindow::updateNotams);
            }
        }
    }
    
    if (ui->notams->model())
        ui->notams->model()->deleteLater();
    
    ui->notams->setModel(m);
}

void FirDetailsWindow::updateBookings()
{
     AtcBookingListModel* m = nullptr;
    
    if (AtcBookingReply* br = qobject_cast<AtcBookingReply*>(sender())) {
        m = new AtcBookingListModel(br->bookings(), this);
    } else {
        auto plugins = PluginFinder::pluginsForIid(qobject_interface_iid<AtcBookingProvider*>());
        if (plugins.length() > 0) {
            AtcBookingProvider* p = qobject_cast<AtcBookingProvider*>(plugins.first());
            Q_ASSERT(p);
            
            const AtcBookingReply* r = p->fetchBookings(m_fir->icao());
            if (r->isFinished())
                m = new AtcBookingListModel(r->bookings(), this);
            else
                connect(r, &AtcBookingReply::finished, this, &FirDetailsWindow::updateBookings);
        } else {
            ui->bookingTab->setEnabled(false);
        }
    }
    
    if (ui->bookings->model())
        ui->bookings->model()->deleteLater();
    
    ui->bookings->setModel(m);
}

}} /* namespace Vatsinator::Widgets */
