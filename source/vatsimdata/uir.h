/*
    uir.h
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

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


#ifndef UIR_H
#define UIR_H

#include <QString>
#include <QVector>

#include "vatsimdata/clickable.h"

class Controller;
class ControllerTableModel;
class Fir;

class Uir : public Clickable {

public:
  /**
   * @param ICAO
   */
  Uir(const QString&);

  /**
   * Reimplemented from Clickable.
   */
  inline Clickable::Type
  objectType() const { return Clickable::UIR; }

  /**
   * @param fir FIR that the UIR is supposed to contain.
   */
  void addFir(Fir*);

  /**
   * @param atc UIR controller.
   */
  void addStaff(const Controller*);
  
  /**
   * Clears the vector.
   */
  void clear();
  
  /**
   * @return True, if there is no ATC on position.
   */
  bool isEmpty() const;
  

  inline const QVector< Fir* > &
  getRange() const { return __range; }

  inline const ControllerTableModel *
  getStaff() const { return __staff; }
  
  inline const QString &
  getIcao() const { return __icao; }
  
  inline const QString &
  getName() const { return __name; }
  
  inline QString &
  getName() { return __name; }

private:
  QString __icao;
  QString __name;
  
  QVector< Fir* > __range;
  ControllerTableModel*  __staff;

};

#endif // UIR_H
