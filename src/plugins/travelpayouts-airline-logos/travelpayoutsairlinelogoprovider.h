/*
 * travelpayoutsairlinelogoprovider.h
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

#ifndef TRAVELPAYOUTSAIRLINELOGOPROVIDER_H
#define TRAVELPAYOUTSAIRLINELOGOPROVIDER_H

#include "misc/airlinelogoprovider.h"
#include <QObject>

/**
 * \defgroup travelpayouts-airline-logo travelpayouts airline logos.
 * @{
 * 
 * https://support.travelpayouts.com/hc/en-us/articles/203956073-Airline-logos
 */
class TravelpayoutsAirlineLogoProvider : public QObject, public Vatsinator::Misc::AirlineLogoProvider {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.eu.vatsinator.vtrmisc.AirlineLogoProvider" FILE "travelpayoutsairlinelogoprovider.json")
    Q_INTERFACES(Vatsinator::Misc::AirlineLogoProvider)

public:
    explicit TravelpayoutsAirlineLogoProvider(QObject* parent = nullptr);
    
    /**
     * \copydoc Vatsinator::Misc::AirlineLogoProvider::fetchLogo()
     */
    const Vatsinator::Misc::AirlineLogoReply* fetchLogo(const Vatsinator::Core::Airline& airline,
                                                        const QSize& prefferedSize) override;
    
};

#endif // TRAVELPAYOUTSAIRLINELOGOPROVIDER_H
