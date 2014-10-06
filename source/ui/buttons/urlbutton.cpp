/*
    urlbutton.cpp
    Copyright (C) 2013-2014  Michał Garapich michal@garapich.pl

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

#include "vatsinatorapplication.h"

#include "urlbutton.h"

UrlButton::UrlButton(const QString& text, const QUrl& url, QWidget* parent) :
    QPushButton(text, parent),
    __url(url) {
  connect(this, SIGNAL(clicked()),
          this, SLOT(__openUrl()));
}

UrlButton::UrlButton(const QString& text, QWidget* parent) :
    QPushButton(text, parent) {
  connect(this, SIGNAL(clicked()),
          this, SLOT(__openUrl()));
}

UrlButton::UrlButton(QWidget* parent) :
    QPushButton(parent) {
  connect(this, SIGNAL(clicked()),
          this, SLOT(__openUrl()));
}

void
UrlButton::setUrl(const QUrl& url) {
  __url = url;
}

void
UrlButton::__openUrl() {
  if (__url.isEmpty())
    qWarning("UrlButton: empty url open requested");
  else
    QDesktopServices::openUrl(__url);
}
