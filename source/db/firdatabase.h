/*
    firdatabase.h
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


#ifndef FIRDATABASE_H
#define FIRDATABASE_H

#include <QVector>
#include <QString>

#include "vatsimdata/fir.h"
#include "singleton.h"

#pragma pack(1)
struct FirHeader {
  char  icao[8];
  int   oceanic; // 0 or 1
  Point externities[2];
  Point textPosition;
};
#pragma pack()



class FirDatabase :
    public QObject,
    public Singleton< FirDatabase > {

  Q_OBJECT

public:
  FirDatabase();

  Fir*  findFirByIcao(const QString&, bool = false);

  void  clearAll();

  inline const QVector< Fir > &
  getFirs() { return __firs; }

private:
  void __readDatabase();

  QVector< Fir >  __firs;

  bool __toolTipsPrepared;

private slots:
  void __init();

};

#endif // FIRDATABASE_H
