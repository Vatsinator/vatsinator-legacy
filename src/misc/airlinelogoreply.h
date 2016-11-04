/*
 * airlinelogoreply.h
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

#ifndef MISC_AIRLINELOGOREPLY_H
#define MISC_AIRLINELOGOREPLY_H

#include "miscexport.h"
#include "core/airline.h"
#include <QObject>
#include <QPixmap>

namespace Vatsinator { namespace Misc {

/**
 * \ingroup Misc
 * @{
 *
 * The AirlineLogoReply class represents a response for airline logo request
 * made to a \ref AirlineLogoProvider implementation instance.
 */
class __VtrMiscApi__ AirlineLogoReply : public QObject {
    Q_OBJECT

    /**
     * The airline.
     */
    Q_PROPERTY(Vatsinator::Core::Airline airline READ airline)
    
signals:
    /**
     * Emitted when the logo is downloaded.
     */
    void finished();
    
public:
    /**
     * Default constructor, represents a logo reply for the \c airline.
     */
    explicit AirlineLogoReply(const Core::Airline& airline, QObject* parent = nullptr);
    
    /**
     * Sets the airline logo to the given pixmap.
     */
    void setLogo(const QPixmap& logo);
    
    /**
     * Returns the airline's logo.
     */
    const QPixmap& logo() const { return m_logo; }
    
    /**
     * Specifies whether the logo is downloaded or not.
     */
    bool isFinished() const { return m_finished; }
    
    /**
     * Sets the \c finished to the given value.
     */
    void setFinished(bool finished);
    
    const Core::Airline& airline() const { return m_airline; }
    
private:
    bool m_finished = false;
    Core::Airline m_airline;
    QPixmap m_logo;

}; /** @} */

}} /* namespace Vatsinator::Misc */

#endif // MISC_AIRLINELOGOREPLY_H
