/*
 * notam.h
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

#ifndef CORE_NOTAM_H
#define CORE_NOTAM_H

#include "miscexport.h"
#include <QDateTime>
#include <QSharedData>
#include <QString>

namespace Vatsinator { namespace Misc {

class NotamData;

/**
 * \ingroup Misc
 * @{
 *
 * The Notam class represents a single NOTAM notice.
 */
class __VtrMiscApi__ Notam {

public:
    /**
     * Flag for the "to" field.
     */
    enum CFlag {
        None,
        Est     /**< Estimated */,
        Perm    /**< Permament */
    };
    
    /**
     * Defines the NOTAM type.
     */
    enum Type {
        New             /**< A new NOTAM */,
        Replacement     /**< Replaces old NOTAM */,
        Cancellation    /**< Clears previously-issued NOTAM */
    };
    
    /**
     * Creates an empty (and invalid) NOTAM.
     */
    Notam();
    
    /**
     * Creates a new NOTAM with the \c ident specified.
     */
    explicit Notam(const QString& ident);
    
    /**
     * Creates a shallow copy of \c orig.
     */
    Notam(const Notam& orig);
    
    /**
     * Destroys the NOTAM.
     */
    virtual ~Notam();
    
    /**
     * Assigns \c other to this NOTAM.
     */
    Notam& operator=(const Notam& other);
    
    /**
     * Returns the NOTAM ident.
     */
    QString ident() const;
    
    /**
     * Returns the ICAO of whom the NOTAM affects.
     */
    QString icao() const;
    
    /**
     * Sets the ICAO code to the given value.
     */
    void setIcao(const QString& icao);
    
    /**
     * Returns the whole notice content.
     */
    QString notam() const;
    
    /**
     * Sets the NOTAM to the given value.
     */
    void setNotam(const QString& notam);
    
    /**
     * Returns URL to where the NOTAM can be read in details.
     */
    QString url() const;
    
    /**
     * Sets the NOTAM's URL to the given value.
     */
    void setUrl(const QString& url);
    
    /**
     * Returns start date and time of the NOTAM.
     */
    QDateTime from() const;
    
    /**
     * Sets start date and time of the NOTAM to the given value.
     */
    void setFrom(const QDateTime& from);
    
    /**
     * Returns finish date and time of the NOTAM.
     */
    QDateTime to() const;
    
    /**
     * Sets finish date and time of the NOTAM to the given value.
     */
    void setTo(const QDateTime& to);
    
    /**
     * Returns the NOTAM's diurnal time if specified, otherwise returns an empty string.
     */
    QString diurnal() const;
    
    /**
     * Sets the NOTAM's diurnal to the given value.
     */
    void setDiurnal(const QString& diurnal);
    
    /**
     * Returns CFlag for the NOTAM.
     */
    CFlag cflag() const;
    
    /**
     * Sets the NOTAM CFlag to the given value.
     */
    void setCflag(CFlag cflag);
    
    /**
     * Returns type of the NOTAM.
     */
    Type type() const;
    
    /**
     * Sets the type of the NOTAM to the given value.
     */
    void setType(Type type);

private:
    QSharedDataPointer<NotamData> d;

}; /** @} */

}} /* namespace Vatsinator::Misc */

Q_DECLARE_TYPEINFO(Vatsinator::Misc::Notam, Q_MOVABLE_TYPE);

#endif // CORE_NOTAM_H
