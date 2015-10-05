/*
 * vatsimevent.h
 * Copyright (C) 2015  Michał Garapich <michal@garapich.pl>
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

#ifndef VATSIMEVENT_H
#define VATSIMEVENT_H

#include <QEvent>
#include <QString>
#include "events/types.h"

/**
 * The VatsimEvent class is used whenever Vatsim-specific action
 * happens.
 */
class VatsimEvent : public QEvent {
    
public:
    /**
     * The Type enum is used to specify type of the event.
     */
    enum Type {
        Message, /**< Vatsim sends a message to user; contents of the
            message can be retrieved by calling \c message() method. */
        StatusError, /**< Error occured during fetching the initial
            status.txt file; it probably means that user does
            not have connection to the internet. */
        DataError /**< Error occured during downloading data update. */
    };
    
    /**
     * Creates a new VatsimEvent.
     * 
     * \param type Type of the event.
     * \param message Optional message.
     */
    VatsimEvent(Type type, const QString& message = QString());
    
    /**
     * Gets the type of the event.
     */
    inline Type type() const
    {
        return __type;
    }
    
    /**
     * Gets the message for the user.
     */
    inline const QString& message() const
    {
        return __message;
    }
    
private:
    Type __type;
    QString __message;
    
};

#endif // VATSIMEVENT_H
