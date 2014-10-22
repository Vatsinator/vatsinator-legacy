/*
 * basewindow.cpp
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

#include <QtWidgets>

#include "ui/windows/vatsinatorwindow.h"
#include "ui/widgetsuserinterface.h"
#include "vatsinatorapplication.h"

#include "basewindow.h"

BaseWindow::BaseWindow(QWidget* parent, Qt::WindowFlags flags) :
  QWidget(parent, flags),
  __windowSetup(false) {}

void BaseWindow::showEvent(QShowEvent*) {
  if (!__windowSetup) {
    this->setGeometry(
      QStyle::alignedRect(
        Qt::LeftToRight,
        Qt::AlignCenter,
        this->size(),
        QDesktopWidget().screenGeometry(wui()->mainWindow())
      )
    );
    
    __windowSetup = true;
  }
}

