/*
    clientdetailsbutton.h
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


#ifndef CLIENTDETAILSBUTTON_H
#define CLIENTDETAILSBUTTON_H

#include <QPushButton>
#include <QIcon>

class Client;

/**
 * The ClientDetailsButton is a convenience class that makes it easy to click
 * a button in order to show client's data.
 */
class ClientDetailsButton : public QPushButton {
  Q_OBJECT
  
signals:
  /**
   * Passed from QPushButton.
   */
  void clicked(const Client* client);

public:
  explicit ClientDetailsButton(const Client* client = nullptr, QWidget* parent = nullptr);
  
  /**
   * Sets the button's pointer to the given value.
   */
  void setClient(const Client* client);

private:
  const Client* __current;

};

#endif // CLIENTDETAILSBUTTON_H
