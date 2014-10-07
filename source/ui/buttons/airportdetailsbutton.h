/*
    airportdetailsbutton.h
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


#ifndef AIRPORTDETAILSBUTTON_H
#define AIRPORTDETAILSBUTTON_H

#include <QPushButton>

class Airport;

/**
 * The AirportDetailsButton class is a button that corresponds to the specified
 * airport. When the button is clicked, the clicked() signal is emitted, so
 * that connecting a slot to it is fairly easy.
 */
class AirportDetailsButton : public QPushButton {
  Q_OBJECT

signals:
  /**
   * Passed from QPushButton.
   */
  void clicked(const Airport* airport);

public:
  /**
   * Creates a new AirportDetailsButton with _airport_ set to _nullptr_.
   */
  AirportDetailsButton(QWidget* parent = nullptr);
  
  /**
   * Create a new AirportDetailsButton with the given _airport_ and
   * _parent_ passed to QPushButton's constructor.
   */
  AirportDetailsButton(const Airport* airport, QWidget* parent = nullptr);

  /**
   * Sets the button's _airport_ to the given object.
   */
  void setAirportPointer(const Airport* airport);

private slots:
  void __handleClicked();

private:
  const Airport* __current;

};

#endif // AIRPORTDETAILSBUTTON_H
