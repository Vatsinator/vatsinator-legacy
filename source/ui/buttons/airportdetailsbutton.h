/*
    airportdetailsbutton.h
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


#ifndef AIRPORTDETAILSBUTTON_H
#define AIRPORTDETAILSBUTTON_H

#include <QPushButton>

class Airport;

class AirportDetailsButton : public QPushButton {

  Q_OBJECT

public:
  AirportDetailsButton(QWidget* = 0);
  AirportDetailsButton(const Airport*, bool = false, QWidget* = 0);

  void setAirportPointer(const Airport*);

signals:
  void clicked(const Airport*);

private slots:
  void __handleClicked();

private:
  const Airport* __current;

};

#endif // AIRPORTDETAILSBUTTON_H
