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

#include "ui/windows/basewindow.h"

#include "ui/ui_airportdetailswindow.h"

class Airport;
class Controller;
class Pilot;
class NotamListModel;
class WeatherForecastModel;

class AirportDetailsWindow :
    public BaseWindow,
    private Ui::AirportDetailsWindow {

  Q_OBJECT

public:
  AirportDetailsWindow(const Airport*, QWidget* = nullptr);
  virtual ~AirportDetailsWindow();
  
protected:
  void showEvent(QShowEvent*) override;

private:
  void __updateModels();
  void __fillLabels();
  void __adjustTables();
  
private slots:
  void __updateForecast();
  void __notamUpdate(NotamListModel*);
  void __goToNotam(QModelIndex);

private:
  const Airport*        __airport;

};

#endif // AIRPORTDETAILSWINDOW_H
