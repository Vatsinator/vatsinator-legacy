/*
    airportdetailsaction.h
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


#ifndef AIRPORTDETAILSACTION_H
#define AIRPORTDETAILSACTION_H

#include <QAction>

#include "vatsimdata/airport.h"

class AirportDetailsAction : public QAction {

  Q_OBJECT

public:
  AirportDetailsAction(const Airport*, const QString&, QObject*);

signals:
  void triggered(const Airport*);

private slots:
  void __handleTriggered();

private:
  const Airport* __current;


};

#endif // AIRPORTDETAILSACTION_H
