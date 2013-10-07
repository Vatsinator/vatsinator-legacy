/*
    defines.h
    This file contains some const variables.
    
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

#ifndef DEFINES_H
#define DEFINES_H

#include "config.h"

// where to get all the data from
#define VATSIM_STATUS_URL "http://status.vatsim.net/status.txt"

// base Vatsinator domain
#define VATSINATOR_BASE_URL "vatsinator.eu.org"

// Vatsinator homepage url
#define VATSINATOR_HOMEPAGE "http://" VATSINATOR_BASE_URL

// Vatsinator_repo url
#define VATSINATOR_REPO_URL "http://repo." VATSINATOR_BASE_URL

// Where stats will be collected
#define VATSINATOR_STATS_URL "http://stats." VATSINATOR_BASE_URL

// Vataware airport url
// %1: ICAO code
#define VATAWARE_AIRPORT_URL "http://www.vataware.com/airport.cfm?airport=%1"

// Vataware pilot url
// %1: Vatsim PID
#define VATAWARE_PILOT_URL "http://www.vataware.com/pilot.cfm?cid=%1"



// if debug, add MemoryTracker to the buildset
#ifndef NO_DEBUG
# define DEBUG_NEW new(__FILE__, __LINE__)
# include "debugging/memorytracker.h"
#else
# define DEBUG_NEW new (std::nothrow)
#endif
#define new DEBUG_NEW

#endif // DEFINES_H
