/*
 * firmanager.h
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#ifndef FIRMANAGER_H
#define FIRMANAGER_H

#include <QObject>

class Fir;

class FirManager : public QObject {
  Q_OBJECT

public:
  explicit FirManager(QObject* = nullptr);
  
  Fir* find(const QString&);
  const Fir* find(const QString&) const;

private:
  void __init();
  
  QVector<Fir*> __firs;

};

#endif // FIRMANAGER_H
