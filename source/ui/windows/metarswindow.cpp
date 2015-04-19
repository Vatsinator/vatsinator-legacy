/*
    metarswindow.cpp
    Copyright (C) 2012-2015  Michał Garapich michal@garapich.pl

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

#include "network/metarupdater.h"
#include "ui/models/metarlistmodel.h"
#include "ui/models/roles.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/widgetsuserinterface.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "metarswindow.h"

MetarsWindow::MetarsWindow(QWidget* parent) :
    QWidget(parent),
    __metars(new MetarListModel(this)),
    __updater(new MetarUpdater(__metars, this)) {
  setupUi(this);
  
  connect(FetchButton, &QPushButton::clicked, this, &MetarsWindow::metarRequested);
  connect(RefreshAllButton, &QPushButton::clicked, __updater, &MetarUpdater::update);
  connect(ClearButton, &QPushButton::clicked, __metars, &MetarListModel::clear);
  connect(MetarIcaoEdit, &QLineEdit::textChanged, this, &MetarsWindow::__handleTextChange);
  connect(__metars,  &MetarListModel::rowsInserted, this, &MetarsWindow::__handleNewMetars);
  connect(vApp()->vatsimDataHandler(), &VatsimDataHandler::vatsimStatusUpdated, this, &MetarsWindow::__enableButtons);
  
  FetchButton->setEnabled(false);
  MetarListView->setModel(__metars);
  MetarListView->setAttribute(Qt::WA_TranslucentBackground);
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

void
MetarsWindow::showEvent(QShowEvent* event) {
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
  auto matches = __metars->match(__metars->index(0), MetarRole, icao, 1);
  if (matches.length() > 0) {
    MetarListView->setCurrentIndex(matches.first());
    MetarListView->scrollTo(matches.first());
  } else {
    if (fetchIfNotFound) {
      __updater->fetch(icao);
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
