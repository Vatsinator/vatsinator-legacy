/*
    metarswindow.cpp
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

#include <QtGui>

#include "network/httphandler.h"

#include "ui/userinterface.h"

#include "vatsimdata/models/metarlistmodel.h"

#include "metarswindow.h"
#include "defines.h"

MetarsWindow::MetarsWindow(QWidget* _parent) :
    QWidget(_parent) {

  setupUi(this);

  UserInterface::setWindowPosition(this);

  __httpHandler = new HttpHandler();

  __metarsHandler = new MetarListModel(__httpHandler);

  connect(FetchButton,    SIGNAL(clicked()),
          this,     SLOT(fetchMetar()));
  connect(RefreshAllButton, SIGNAL(clicked()),
          __metarsHandler,  SLOT(updateAllMetars()));
  connect(ClearButton,    SIGNAL(clicked()),
          __metarsHandler,  SLOT(clear()));
  connect(MetarICAO,    SIGNAL(textChanged(const QString&)),
          this,     SLOT(__handleTextChange(const QString&)));
  
  FetchButton->setEnabled(false);
  MetarsDisplay->setModel(__metarsHandler);
  
  MetarICAO->setFocus();
}

MetarsWindow::~MetarsWindow() {
  delete __metarsHandler;
  delete __httpHandler;
}

void
MetarsWindow::show(QString _icao) {
  QWidget::show();
  __metarsHandler->fetchMetar(_icao);
}

void
MetarsWindow::fetchMetar() {
  __metarsHandler->fetchMetar(MetarICAO->text());
  MetarICAO->setText("");
}

void
MetarsWindow::keyPressEvent(QKeyEvent* _event) {
  if (!MetarICAO->text().isEmpty() &&
      (_event->key() == Qt::Key_Return || _event->key() == Qt::Key_Enter))
    fetchMetar();
  
  QWidget::keyPressEvent(_event);
}

void
MetarsWindow::__handleTextChange(const QString& _text) {
  if (_text.length() == 0)
    FetchButton->setEnabled(false);
  else
    FetchButton->setEnabled(true);
}

