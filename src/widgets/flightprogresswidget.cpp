/*
 * flightprogresswidget.cpp
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

#include "flightprogresswidget.h"
#include <QtWidgets>

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Widgets {

FlightProgressWidget::FlightProgressWidget(QWidget* parent) :
    QProgressBar(parent)
{
    setMaximum(100);
    setTextVisible(false);
}

void FlightProgressWidget::setFlight(const Pilot* flight)
{
    if (!m_flight.isNull())
        disconnect(m_flight.data());
    
    m_flight = QPointer<const Pilot>(flight);
    connect(m_flight.data(), &Pilot::progressChanged, this, &FlightProgressWidget::progressChanged);
    setValue(m_flight->progress());
}

void FlightProgressWidget::progressChanged()
{
    setValue(m_flight->progress());
}

}} /* namespace Vatsinator::Widgets */
