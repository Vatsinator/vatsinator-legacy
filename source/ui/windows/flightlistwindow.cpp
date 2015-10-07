/*
    flightlistwindow.cpp
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

#include "models/flighttablemodel.h"
#include "models/roles.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/widgetsuserinterface.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/pilot.h"
#include "vatsinatorapplication.h"

#include "flightlistwindow.h"

FlightListWindow::FlightListWindow(QWidget* parent) :
    QWidget(parent)
{
    setupUi(this);
    FlightsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    connect(FlightsTable, &QTableView::doubleClicked, [this](const QModelIndex& index) {
        Q_ASSERT(index.data(InstancePointerRole).isValid());
        Client* const client = reinterpret_cast<Client* const>(index.data(InstancePointerRole).value<void*>());
        wui()->showClientDetails(client);
    });
    
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), QDesktopWidget().screenGeometry(wui()->mainWindow())));
}

void
FlightListWindow::showEvent(QShowEvent* event)
{
    FlightsTable->setModel(vApp()->vatsimDataHandler()->flights());
    Q_UNUSED(event);
}
