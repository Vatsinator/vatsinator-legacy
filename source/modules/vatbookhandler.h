/*
    vatbookhandler.h
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


#ifndef VATBOOKHANDLER_H
#define VATBOOKHANDLER_H

#include <QMap>
#include <QString>

#include "singleton.h"

class BookedAtcTableModel;

class VatbookHandler :
    public Singleton< VatbookHandler > {
  
  /*
   * This class privides VATBOOK integration.
   */
  
public:
  VatbookHandler();
  virtual ~VatbookHandler();
  
  inline BookedAtcTableModel *
  getModel(const QString& _icao) {
    return __bookings.value(_icao, __bookings["ZZZZ"]); 
  }
  
private:
  /* This map contains pairs airport/fir icao - atcs */
  QMap< QString, BookedAtcTableModel* > __bookings;
  
  
};

#endif // VATBOOKHANDLER_H
