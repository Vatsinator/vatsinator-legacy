/*
    urlbutton.cpp
    Copyright (C) 2013  Michał Garapich michal@garapich.pl

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

#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
# include <QtWidgets>
#else
# include <QtGui>
#endif

#include "vatsinatorapplication.h"

#include "urlbutton.h"

UrlButton::UrlButton(const QString& _text, const QString& _url, QWidget* _parent) :
    QPushButton(_text, _parent),
    __url(_url) {
  connect(this, SIGNAL(clicked()),
          this, SLOT(__openUrl()));
}

UrlButton::UrlButton(const QString& _text, QWidget* _parent) :
    QPushButton(_text, _parent) {
  connect(this, SIGNAL(clicked()),
          this, SLOT(__openUrl()));
}

UrlButton::UrlButton(QWidget* _parent) :
    QPushButton(_parent) {
  connect(this, SIGNAL(clicked()),
          this, SLOT(__openUrl()));
}

void
UrlButton::setUrl(const QString& _url) {
  __url = _url;
}

void
UrlButton::__openUrl() {
  if (__url.isEmpty())
    VatsinatorApplication::log("UrlButton: empty url open requested");
  else
    QDesktopServices::openUrl(__url);
}
