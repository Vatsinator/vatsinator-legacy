/*
    modulesmanager.h
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


#ifndef MODULESMANAGER_H
#define MODULESMANAGER_H

#include "singleton.h"

class FlightTracker;
class ModelsMatcher;

class ModulesManager : public Singleton< ModulesManager > {
	
public:
	ModulesManager();
	virtual ~ModulesManager();
	
	void initBeforeGL();
	void initAfterGL();
	
private:
	FlightTracker * __flightTracker;
	ModelsMatcher *	__modelsMatcher;
};

#endif // MODULESMANAGER_H
