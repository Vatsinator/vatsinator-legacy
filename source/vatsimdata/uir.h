/*
    uir.h
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


#ifndef UIR_H
#define UIR_H

#include <QObject>
#include <QList>

class Controller;
class ControllerTableModel;
class Fir;

/**
 * Upper FIR.
 */
class Uir : public QObject {
  Q_OBJECT
  
signals:
  void updated();

public:
  /**
   * \param icao The ICAO code.
   * \param parent Passed to QObject's constructor.
   */
  Uir(const QString& icao, QObject* parent = nullptr);

  /**
   * \param fir FIR that the UIR is supposed to contain.
   */
  void addFir(Fir* fir);

  /**
   * \param atc UIR controller.
   */
  void addStaff(const Controller* atc);
  
  /**
   * \return True, if there is no ATC on position.
   */
  bool isEmpty() const;
  
  inline const QList<Fir*>& range() const { return __range; }
  inline const ControllerTableModel* staff() const { return __staff; }
  inline const QString& icao() const { return __icao; }
  inline const QString& name() const { return __name; }
  inline QString& name() { return __name; }

private:
  QString __icao;
  QString __name;
  
  QList<Fir*>         __range;
  ControllerTableModel* __staff;

};

#endif // UIR_H
