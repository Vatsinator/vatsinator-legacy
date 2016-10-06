/*
 * clientlistswindow.cpp
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

#include "clientlistwindow.h"
#include "ui_clientlistwindow.h"
#include "widgetsprivate.h"
#include "misc/atclistmodel.h"
#include "misc/flightlistmodel.h"
#include "misc/roles.h"
#include <QtWidgets>

using namespace Vatsinator::Core;
using namespace Vatsinator::Misc;

namespace Vatsinator { namespace Widgets {

ClientListWindow::ClientListWindow(const ServerTracker* server, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientListWindow)
{
    ui->setupUi(this);

    connect(ui->flights, &QAbstractItemView::doubleClicked, this, &ClientListWindow::showDetails);
    ui->flights->setModel(FlightListModel::all(server));
    ui->flights->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->atcs, &QAbstractItemView::doubleClicked, this, &ClientListWindow::showDetails);
    ui->atcs->setModel(AtcListModel::all(server));
    ui->atcs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

ClientListWindow::~ClientListWindow() {}

void ClientListWindow::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);
    fixupGeometry(this);
}

void ClientListWindow::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void ClientListWindow::showDetails(const QModelIndex& index)
{
    QString callsign = index.data(Misc::CallsignRole).toString();
    Q_ASSERT(!callsign.isEmpty());
    emit clientDetailsRequested(callsign);
}

}} /* namespace Vatsinator::Widgets */
