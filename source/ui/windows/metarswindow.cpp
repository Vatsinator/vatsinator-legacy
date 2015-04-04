/*
    metarswindow.cpp
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

#include <QtWidgets>

#include "network/plaintextdownloader.h"
#include "ui/models/metarlistmodel.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/widgetsuserinterface.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "metarswindow.h"

MetarsWindow::MetarsWindow(QWidget* parent) :
    QWidget(parent) {
  setupUi(this);
  
  connect(qApp, SIGNAL(aboutToQuit()),
          this, SLOT(hide()));

  __httpHandler = new PlainTextDownloader();

  __metarsHandler = new MetarListModel(__httpHandler);

  connect(FetchButton,      SIGNAL(clicked()),
          this,             SLOT(metarRequested()));
  connect(RefreshAllButton, SIGNAL(clicked()),
          __metarsHandler,  SLOT(updateAll()));
  connect(ClearButton,      SIGNAL(clicked()),
          __metarsHandler,  SLOT(clear()));
  connect(MetarIcaoEdit,    SIGNAL(textChanged(const QString&)),
          this,             SLOT(__handleTextChange(const QString&)));
  connect(__metarsHandler,  SIGNAL(newMetarsAvailable()),
          this,             SLOT(__handleNewMetars()));
  connect(vApp()->vatsimDataHandler(),  SIGNAL(vatsimDataUpdated()),
          this,                         SLOT(__enableButtons()));
  
  FetchButton->setEnabled(false);
  MetarListView->setModel(__metarsHandler);
  MetarListView->setAttribute(Qt::WA_TranslucentBackground);
}

MetarsWindow::~MetarsWindow() {
  delete __metarsHandler;
  delete __httpHandler;
}

void
MetarsWindow::show(QString icao) {
  QWidget::show();
  __findAndSelectMetar(icao);
}

void
MetarsWindow::metarRequested() {
  __findAndSelectMetar(MetarIcaoEdit->text());
}

void MetarsWindow::showEvent(QShowEvent* event) {
  if (!event->spontaneous()) {
    QRect rect = QDesktopWidget().screenGeometry(wui()->mainWindow());
    int m = rect.height() / 4;
    
    rect.setTop(rect.top() + m);
    rect.setBottom(rect.bottom() - m);
    
    this->setGeometry(
      QStyle::alignedRect(
        Qt::LeftToRight,
        Qt::AlignTop | Qt::AlignHCenter,
        this->size(),
        rect
      )
    );
    MetarListView->setMaximumHeight(m * 2);
  }
  MetarIcaoEdit->setFocus();
  
  QWidget::showEvent(event);
}

void
MetarsWindow::keyPressEvent(QKeyEvent* event) {
  if (!MetarIcaoEdit->text().isEmpty() && MetarIcaoEdit->hasFocus() &&
      (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter))
    metarRequested();
  
  QWidget::keyPressEvent(event);
}

void
MetarsWindow::__findAndSelectMetar(const QString& icao, bool fetchIfNotFound) {
  const Metar* m = __metarsHandler->find(icao.toUpper());
  if (m) {
    const QModelIndex mi = __metarsHandler->modelIndexForMetar(m);
    MetarListView->setCurrentIndex(mi);
    MetarListView->scrollTo(mi);
  } else {
    if (fetchIfNotFound) {
      __metarsHandler->fetchMetar(icao);
      __awaited = icao;
    }
  }
  
  MetarIcaoEdit->clear();
}

void
MetarsWindow::__handleTextChange(const QString& text) {
  FetchButton->setEnabled(text.length() != 0);
}

void
MetarsWindow::__handleNewMetars() {
  if (!__awaited.isEmpty()) {
    __findAndSelectMetar(__awaited, false);
    __awaited.clear();
  }
}

void
MetarsWindow::__enableButtons() {
  FetchButton->setEnabled(true);
  RefreshAllButton->setEnabled(true);
}
