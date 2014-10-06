/*
    airportdetailsaction.h
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


#ifndef AIRPORTDETAILSACTION_H
#define AIRPORTDETAILSACTION_H

#include <QAction>
#include "vatsimdata/airport.h"

/**
 * The AirportDetailsAction represents an action in menu that corresponds
 * to the Airport class instance.
 */
class AirportDetailsAction : public QAction {
  Q_OBJECT

signals:
  /**
   * The triggered() signal is forwarded from QAction's.
   */
  void triggered(const Airport* airport);

public:
  /**
   * Creates new action with the given _airport_, _label_ and _parent_.
   */
  AirportDetailsAction(const Airport* airport, const QString& label,
                       QObject* parent);

private:
  const Airport* __current;


};

#endif // AIRPORTDETAILSACTION_H
