/*
    aboutwindow.cpp
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

#include "aboutwindow.h"
#include "defines.h"

#include "ui/about.h"
#include "ui/license.h"


AboutWindow::AboutWindow(QWidget* _parent) : QWidget(_parent) {
  setupUi(this);
  __setWindowPosition();
  AuthorsField->setHtml(trUtf8(ABOUT_TEXT));
  LicenseField->setHtml("<pre>" + trUtf8(LICENSE_TEXT) + "</pre>");
  VersionLabel->setText("Version " VATSINATOR_VERSION);
}

void
AboutWindow::__setWindowPosition() {
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
