/*
    uir.h
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


#ifndef UIR_H
#define UIR_H

#include <QString>
#include <QVector>

class Controller;
class ControllerTableModel;
class Fir;

class Uir {

public:
  /**
   * @param ICAO
   */
  Uir(const QString&);
  
  virtual ~Uir();

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
  
  inline const QVector<Fir*>& range() const { return __range; }
  inline const ControllerTableModel* staffModel() const { return __staff; }
  inline const QString& icao() const { return __icao; }
  inline const QString& name() const { return __name; }
  inline QString& name() { return __name; }

private:
  QString __icao;
  QString __name;
  
  QVector<Fir*>         __range;
  ControllerTableModel* __staff;

};

#endif // UIR_H
