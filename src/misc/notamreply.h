/*
 * notamreply.h
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

#ifndef MISC_NOTAMREPLY_H
#define MISC_NOTAMREPLY_H

#include "misc/notam.h"
#include "misc/v8rmisc_export.h"
#include <QtCore/QObject>

namespace Vatsinator { namespace Misc {

/**
 * \ingroup Misc
 * @{
 * 
 * The NotamReply class represents a response to a NOTAM request and is used
 * by \ref NotamProvider implementations.
 */
class V8RMISC_EXPORT NotamReply : public QObject {
    Q_OBJECT
    
    /**
     * The ICAO code of the NOTAM.
     */
    Q_PROPERTY(QString icao READ icao)
    
signals:
    /**
     * Indicates the moment when a \c NOTAM list was succesfully fetched.
     */
    void finished();

public:
    /**
     * Creates a new NotamReply instance that represents a NOTAM response
     * for the given ICAO code.
     */
    explicit NotamReply(const QString& icao, QObject* parent = nullptr);
    
    /**
     * Returns list of NOTAMs for the given ICAO code.
     */
    const QList<Notam>& notams() const { return m_notams; }
    
    /**
     * Adds a new NOTAM to the list.
     */
    void addNotam(const Notam& notam);
    
    /**
     * Specifies whether the NOTAM list is ready or not.
     */
    bool isFinished() const { return m_finished; }
    
    /**
     * Sets the \c finished to the given value.
     */
    void setFinished(bool finished);
    
    const QString& icao() const { return m_icao; }
    
private:
    bool m_finished = false;
    QString m_icao;
    QList<Notam> m_notams;
    
}; /** @} */

}} /* namespace Vatsinator::Misc */

#endif // MISC_NOTAMREPLY_H
