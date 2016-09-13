/*
 * airlinelogoprovider.h
 * Copyright (C) 2016 Michał Garapich <michal@garapich.pl>
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

#ifndef MISC_AIRLINELOGOPROVIDER_H
#define MISC_AIRLINELOGOPROVIDER_H

#include "miscexport.h"
#include "misc/airlinelogoreply.h"
#include <QString>
#include <QtPlugin>

namespace Vatsinator { namespace Misc {

/**
 * \ingroup Misc
 * @{
 */
class __VtrMiscApi__ AirlineLogoProvider {
public:
    /**
     * Default constructor.
     */
    AirlineLogoProvider() {}

    /**
     * Retrieves logo for the \c airline.
     */
    virtual const AirlineLogoReply* fetchLogo(const Core::Airline& airline,
                                              const QSize& prefferedSize) = 0;

}; /** @} */

}} /* namespace Vatsinator::Misc */

Q_DECLARE_INTERFACE(Vatsinator::Misc::AirlineLogoProvider, "org.eu.vatsinator.vtrmisc.AirlineLogoProvider")

#endif // MISC_AIRLINELOGOPROVIDER_H
