/*
    airportdetailsbutton.cpp
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl

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

#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
# include <QtWidgets>
#else
# include <QtGui>
#endif

#include "ui/userinterface.h"
#include "vatsimdata/airport.h"
#include "vatsinatorapplication.h"

#include "airportdetailsbutton.h"

AirportDetailsButton::AirportDetailsButton(QWidget* _parent) :
    QPushButton("", _parent),
    __current(NULL) {
  connect(this, SIGNAL(clicked()), this, SLOT(__handleClicked()));
  // we assume that autoconnect is on
  connect(this,                         SIGNAL(clicked(const Airport*)),
          vApp()->userInterface(),      SLOT(showDetails(const Airport*)));
}

AirportDetailsButton::AirportDetailsButton(const Airport* _ap,
                                     bool _autoconnect,
                                     QWidget* _parent) :
    QPushButton("", _parent),
    __current(_ap) {
  setIcon(QIcon(":/uiIcons/button-details.png"));
  
  connect(this, SIGNAL(clicked()), this, SLOT(__handleClicked()));

  if (_autoconnect)
    connect(this,                       SIGNAL(clicked(const Airport*)),
            vApp()->userInterface(),    SLOT(showDetails(const Airport*)));
}

void
AirportDetailsButton::setAirportPointer(const Airport* _ap) {
  __current = _ap;
  setEnabled(_ap != nullptr);
  setFlat(_ap == nullptr);
}

void
AirportDetailsButton::__handleClicked() {
  if (__current)
    emit clicked(__current);
}
