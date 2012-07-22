/*
    atcdetailswindow.cpp
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

#include <QtGui>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"

#include "ui/mapwidget/mapwidget.h"

#include "vatsimdata/controller.h"

#include "atcdetailswindow.h"
#include "defines.h"

ATCDetailsWindow::ATCDetailsWindow(QWidget* _parent) :
    QWidget(_parent) {
  setupUi(this);
  __setWindowPosition();

  connect(ShowButton, SIGNAL(clicked()), this, SLOT(__handleShowClicked()));
}

void
ATCDetailsWindow::show(const Client* _client) {
  Q_ASSERT(dynamic_cast< const Controller* >(_client));
  __current = dynamic_cast< const Controller* >(_client);

  setWindowTitle(QString(__current->callsign + " - ATC details"));

  CallsignLabel->setText(__current->callsign);
  FacilityLabel->setText(__current->description);
  NameLabel->setText(__current->realName);
  FrequencyLabel->setText(__current->frequency);
  RatingLabel->setText(Controller::ratings[__current->rating]);

  if (__current->airport)
    AirportLabel->setText(static_cast< QString >(__current->airport->icao) %
                          " " %
                          QString::fromUtf8(__current->airport->name) %
                          ", " %
                          QString::fromUtf8(__current->airport->city)
                         );
  else
    AirportLabel->setText("N/A");

  ServerLabel->setText(__current->server);
  TimeOnlineLabel->setText(__current->onlineFrom.toString("dd MMM yyyy, hh:mm"));

  AtisMessageField->setPlainText(__current->atis);

  if (!isVisible())
    QWidget::show();
  else
    activateWindow();
}

void
ATCDetailsWindow::__setWindowPosition() {
  QDesktopWidget* desktop = QApplication::desktop();

  int screenWidth, width;
  int screenHeight, height;

  int x, y;

  QSize windowSize;

  screenWidth = desktop -> width();
  screenHeight = desktop -> height();

  windowSize = size();
  width = windowSize.width();
  height = windowSize.height();

  x = (screenWidth - width) / 2;
  y = (screenHeight - height) / 2;
  y -= 50;

  move(x, y);
}

void
ATCDetailsWindow::__handleShowClicked() {
  Q_ASSERT(__current);
  MapWidget::GetSingleton().showClient(__current);
}

