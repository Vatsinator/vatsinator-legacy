/*
    firdetailswindow.h
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


#ifndef FIRDETAILSWINDOW_H
#define FIRDETAILSWINDOW_H

#include "ui/windows/basewindow.h"

#include "ui/ui_firdetailswindow.h"

class Airport;
class Controller;
class Fir;
class Pilot;
class NotamListModel;

class FirDetailsWindow :
    public BaseWindow,
    private Ui::FirDetailsWindow {

  Q_OBJECT

public:
  FirDetailsWindow(const Fir* fir, QWidget* parent = nullptr);

public slots:
  void show();

private:
  void __updateModels();
  void __fillLabels();
  void __adjustTables();
  
private slots:
  void __notamUpdate(NotamListModel* model);
  void __goToNotam(QModelIndex index);
  
private:
  const Fir* __fir;

};

#endif // FIRDETAILSWINDOW_H
