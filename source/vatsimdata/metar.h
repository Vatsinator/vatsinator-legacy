/*
    metar.h
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


#ifndef METAR_H
#define METAR_H

#include <QString>
#include <QStringBuilder>
#include <QDateTime>

/**
 * The Metar class represents a single METAR report.
 * 
 * \todo Parse METARs.
 */
class Metar {
  
public:
  /**
   * Creates an empty report.
   */
  Metar();
  
  /**
   * Creates new METAR for _icao_ with the given _metar_.
   */
  Metar(const QString& icao, const QString& metar = QString());
  
  /**
   * Sets the METAR report to the given _metar_.
   */
  void setMetar(const QString& metar);
  
  inline const QString& icao() const { return __icao; }
  inline const QString& metar() const { return __metar; }
  inline const QString lastFetchTime() const {
    return __lastFetchTime.toString("dd MMM yyyy, hh:mm") % " UTC";
  }
  
private:
  QString   __icao;
  QString   __metar;
  QDateTime __lastFetchTime;
  
};

#endif // METAR_H
