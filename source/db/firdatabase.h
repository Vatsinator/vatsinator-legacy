/*
    firdatabase.h
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


#ifndef FIRDATABASE_H
#define FIRDATABASE_H

#include <QCoreApplication>
#include <QVector>
#include <QString>

#include "db/point.h"

#include "vatsimdata/fir.h"
#include "singleton.h"

class Fir;

#pragma pack(1)
struct FirHeader {
  char  icao[8];
  int   oceanic; // 0 or 1
  Point externities[2];
  Point textPosition;
};
#pragma pack()



class FirDatabase : public QObject, public Singleton<FirDatabase> {
  
  Q_OBJECT
  
signals:
  void fatal(QString);

public:
  FirDatabase(QObject* = nullptr);

  /**
   * Finds FIR by given ICAO.
   * @param icao ICAO code.
   * @param fss If true, will dinf only FSS FIRs. Default: false
   * @return FIR if any found, otherwise NULL.
   */
  Fir* find(const QString&, bool = false);

  /**
   * Calls Fir::clear() on every Fir.
   */
  void  clearAll();

  inline QVector<Fir>& firs() { return __firs; }
  inline const QVector<Fir>& firs() const { return __firs; }

private:
  void __readDatabase();

  QVector<Fir>  __firs;

};

#endif // FIRDATABASE_H
