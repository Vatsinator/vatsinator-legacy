/*
    clientdetailsaction.h
    Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl

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


#ifndef CLIENTDETAILSACTION_H
#define CLIENTDETAILSACTION_H

#include <QAction>
#include "vatsimdata/client.h"

/**
 * The ClientDetailsAction represents an action in the menu that corresponds
 * to the specified Client.
 */
class ClientDetailsAction : public QAction {
  Q_OBJECT

signals:
  /**
   * Passed from QAction.
   */
  void triggered(const Client* client);

public:
  /**
   * Creates new ClientDetailsAction with the specified _client_ and _label_.
   * Passes _parent_ to QAction's constructor.
   */
  ClientDetailsAction(const Client* client, const QString& label, QObject* parent);

private:
  const Client* __current;


};

#endif // CLIENTDETAILSACTION_H
