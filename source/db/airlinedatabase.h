/*
 * airlinedatabase.h
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef AIRLINEDATABASE_H
#define AIRLINEDATABASE_H

#include <QCoreApplication>
#include <QString>
#include <QMap>

#include "singleton.h"

class AirlineDatabase : public Singleton<AirlineDatabase> {
  
  Q_DECLARE_TR_FUNCTIONS(AirlineDatabase);
  
public:
  AirlineDatabase();
  
  const QString find(const QString&);
  
private:
  void __init();
  
  QMap<QString, QString> __airlines;
  
#ifdef GCC_VERSION_47
  const QString __nope = "";
#else
  const QString __nope;
#endif

};

#endif // AIRLINEDATABASE_H
