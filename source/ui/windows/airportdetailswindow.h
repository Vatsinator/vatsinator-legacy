/*
    airportdetailswindow.h
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


#ifndef AIRPORTDETAILSWINDOW_H
#define AIRPORTDETAILSWINDOW_H

#include "ui/ui_airportdetailswindow.h"
#include "singleton.h"

class Airport;
class Controller;
class Pilot;

class AirportDetailsWindow :
    public QWidget,
    public Singleton< AirportDetailsWindow >,
    private Ui::AirportDetailsWindow {

  Q_OBJECT

public:
  AirportDetailsWindow(QWidget* = 0);

public slots:
  void show(const Airport*);
  void updateMetar();
  void updateMetar(QString);

private:
  void __updateModels(const Airport* = NULL);
  void __fillLabels(const Airport*);
  void __adjustTables();

  QString __currentICAO;
  const Airport* __current;

private slots:
  void __updateData();
  void __handleShowClicked();

};

#endif // AIRPORTDETAILSWINDOW_H
