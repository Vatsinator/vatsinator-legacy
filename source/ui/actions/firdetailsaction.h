/*
    firdetailsaction.h
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


#ifndef FIRDETAILSACTION_H
#define FIRDETAILSACTION_H

#include <QAction>
#include "vatsimdata/fir.h"

/**
 * The FirDetailsAction represents a menu action that corresponds
 * with the specified Fir instance.
 */
class FirDetailsAction : public QAction {
  Q_OBJECT

signals:
  /**
   * Passed from QAction.
   */
  void triggered(const Fir* fir);

public:
  /**
   * Creates new FirDetailsAction with the specified _fir_ and _label_.
   * _parent_ is passed to the QAction's constructor.
   */
  FirDetailsAction(const Fir* fir, const QString& label, QObject* parent);

private:
  const Fir* __current;

};

#endif // FIRDETAILSACTION_H
