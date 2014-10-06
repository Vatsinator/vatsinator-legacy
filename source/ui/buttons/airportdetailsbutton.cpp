/*
    airportdetailsbutton.cpp
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

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

#include "ui/userinterface.h"
#include "vatsimdata/airport.h"
#include "vatsinatorapplication.h"

#include "airportdetailsbutton.h"

AirportDetailsButton::AirportDetailsButton(QWidget* parent) :
    QPushButton(parent),
    __current(nullptr) {
  connect(this, SIGNAL(clicked()), this, SLOT(__handleClicked()));
}

AirportDetailsButton::AirportDetailsButton(const Airport* airport, QWidget* parent) :
    QPushButton(parent),
    __current(airport) {
//   setIcon(QIcon(":/uiIcons/button-details.png"));
//   
  connect(this, SIGNAL(clicked()), this, SLOT(__handleClicked()));
}

void
AirportDetailsButton::setAirportPointer(const Airport* airport) {
  __current = airport;
  setEnabled(airport != nullptr);
  setFlat(airport == nullptr);
}

void
AirportDetailsButton::__handleClicked() {
  if (__current)
    emit clicked(__current);
}
