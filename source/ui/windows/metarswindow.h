/*
    metarswindow.h
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

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


#ifndef METARSWINDOW_H
#define METARSWINDOW_H

#include <QVector>

#include "ui/ui_metarswindow.h"
#include "singleton.h"
#include "vatsimdata/metar.h"

class VatsimDataHandler;
class HttpHandler;
class MetarListModel;

class MetarsWindow :
    public QWidget,
    public Singleton< MetarsWindow >,
    private Ui::MetarsWindow {

  /*
   * This class handles our "METARs" window. Shown when Vatsinator->
   * ->Metar is activated.
   */

  Q_OBJECT

public:
  MetarsWindow(QWidget* = 0);
  virtual ~MetarsWindow();

public slots:
  void show(QString);
  void fetchMetar();

private:
  void __setWindowPosition();

  HttpHandler*  __httpHandler;
  MetarListModel*   __metarsHandler;

private slots:
  void __handleTextChange(const QString&);




};

#endif // METARSWINDOW_H
