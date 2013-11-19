/*
 * databasewindow.cpp
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QtGui>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"

#include "databasewindow.h"
#include "defines.h"

DatabaseWindow::DatabaseWindow(QWidget* _parent) :
    BaseWindow(_parent) {
  setupUi(this);
  
  DatabaseInfoLabel->setText(
    tr(
      "Your local clone of Vatsinator Database contains %n airport(s)",
      "The first part of the summary", AirportDatabase::getSingleton().airports().count()
    ) % " " %
    tr(
      "and %n FIR(s).",
      "The second part of the summary", FirDatabase::getSingleton().firs().count()
    )
  );
}
