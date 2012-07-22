/*
    modulemanager.h
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

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


#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H

#include "singleton.h"

#include <QObject>

class FlightTracker;
class ModelMatcher;

class ModuleManager : public QObject, public Singleton< ModuleManager > {

  Q_OBJECT

public:
  ModuleManager();
  virtual ~ModuleManager();

  void init();

private slots:
  void __initAfterGL();

private:
  FlightTracker* __flightTracker;
  ModelMatcher*   __modelsMatcher;
};

#endif // MODULEMANAGER_H
