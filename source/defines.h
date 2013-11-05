/*
    defines.h
    This file contains global build configuration.
    
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

// if debug, add MemoryTracker to the buildset
#ifndef NO_DEBUG
# define DEBUG_NEW new(__FILE__, __LINE__)
# include "debugging/memorytracker.h"
#else
# define DEBUG_NEW new (std::nothrow)
#endif
#define new DEBUG_NEW

#endif // DEFINES_H
