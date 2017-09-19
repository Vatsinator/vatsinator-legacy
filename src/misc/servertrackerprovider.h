/*
 * servertrackerprovider.h
 * Copyright (C) 2017  Michał Garapich <michal@garapich.pl>
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

#ifndef MISC_SERVERTRACKERPROVIDER_H
#define MISC_SERVERTRACKERPROVIDER_H

#include "core/servertracker.h"
#include <QtCore/QObject>

namespace Vatsinator { namespace Misc {

class ServerTrackerProvider {

public:
    virtual Vatsinator::Core::ServerTracker* serverTracker() = 0;

};

}} /* namespace Vatsinator::Misc */

Q_DECLARE_INTERFACE(Vatsinator::Misc::ServerTrackerProvider, "org.eu.vatsinator.v8rmisc.ServerTrackerProvider")

#endif // MISC_SERVERTRACKERPROVIDER_H
