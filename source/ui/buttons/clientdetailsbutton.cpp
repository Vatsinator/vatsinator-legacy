/*
    clientdetailsbutton.cpp
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

#include "vatsimdata/client.h"

#include "clientdetailsbutton.h"

ClientDetailsButton::ClientDetailsButton(const Client* _client,
                                         QWidget* _parent) :
    QPushButton("", _parent),
    __current(_client) {
  
  static const QIcon iIcon(":/uiIcons/button-details.png");
  
  setIcon(iIcon);
  connect(this, SIGNAL(clicked()),
          this, SLOT(__handleClicked()));
}

void
ClientDetailsButton::setClient(const Client* _client) {
  __current = _client;
}

void
ClientDetailsButton::__handleClicked() {
  emit clicked(__current);
}



