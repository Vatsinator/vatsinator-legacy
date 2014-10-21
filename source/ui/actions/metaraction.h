/*
    metaraction.h
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


#ifndef METARACTION_H
#define METARACTION_H

#include <QAction>
#include <QString>

/**
 * The MetarAction class represents an action that corresponds to the specified
 * METAR report.
 */
class MetarAction : public QAction {
  Q_OBJECT

signals:
  /**
   * Passed from QAction.
   * 
   * \param metar The ICAO code of the airport.
   */
  void triggered(QString metar);

public:
  /**
   * Creates new menu action with the provided _icao_ code and the specified
   * _parent_, which is passed to QAction's constructor.
   */
  MetarAction(const QString& icao, QObject* parent);

private:
  QString __icao;

};

#endif // METARACTION_H
