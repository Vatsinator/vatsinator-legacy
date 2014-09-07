/*
    atcdetailswindow.h
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


#ifndef ATCDETAILSWINDOW_H
#define ATCDETAILSWINDOW_H

#include <QWidget>
#include "ui/ui_atcdetailswindow.h"

class Client;
class Controller;

class AtcDetailsWindow : public QWidget, private Ui::ATCDetailsWindow {
  Q_OBJECT

public:
  AtcDetailsWindow(const Controller*, QWidget* = nullptr);

protected:
  virtual void showEvent(QShowEvent*) override;

private:
  const Controller* __atc;

private slots:
  void __updateLabels();

};

#endif // ATCDETAILSWINDOW_H
