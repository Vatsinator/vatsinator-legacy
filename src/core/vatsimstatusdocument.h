/*
 * vatsimstatusdocument.h
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

#ifndef CORE_VATSIMSTATUSDOCUMENT_H
#define CORE_VATSIMSTATUSDOCUMENT_H

#include "coreexport.h"
#include <QList>
#include <QMetaType>
#include <QUrl>
#include <QSharedDataPointer>

namespace Vatsinator { namespace Core {

class VatsimStatusDocumentData;

/**
 * \ingroup Core
 * @{
 * 
 * The VatsimStatusDocument class represents a status.txt file that is
 * downloaded from VATSIM servers at the program startup.
 * 
 * \sa VatsimStatusReader.
 */
class __VtrCoreApi__ VatsimStatusDocument {
public:
    /**
     * Creates an empty and invalid document.
     */
    VatsimStatusDocument();
    
    /**
     * Creates a copy of \c other.
     */
    VatsimStatusDocument(const VatsimStatusDocument& other);
    
    /**
     * Assigns \c other to this instance and returns a reference to self.
     */
    VatsimStatusDocument& operator =(const VatsimStatusDocument& other);
    
    virtual ~VatsimStatusDocument();
    
    /**
     * URL where to retrieve a more updated status.txt file that overrides this
     * one (the \c moveto0 directive).
     */
    QUrl moveTo() const;
    
    /**
     * Sets the \c moveTo value to the specified one.
     */
    void setMoveTo(const QUrl& moveTo);
    
    /**
     * Message to be displayed at application startup (the \c msg0 directive);
     */
    QString message() const;
    
    /**
     * Sets the \c message to the given value.
     */
    void setMessage(const QString& message);
    
    /**
     * URL where to retrieve METARs (the \c metar0 directive).
     */
    QUrl metar() const;
    
    /**
     * Sets the \c metar URL to the given value.
     */
    void setMetar(const QUrl& metar);
    
    /**
     * URLs where servers list data files are available (\c url1 directives).
     */
    QList<QUrl> serversListUrls() const;
    
    /**
     * Sets the list of URLs to servers list data files to the given value.
     */
    void setServersListUrls(QList<QUrl> serversListUrls);
    
    /**
     * URLs where complete data files are available (\c url0 directives).
     */
    QList<QUrl> dataFileUrls() const;
    
    /**
     * Sets the list of URLs where complete data files are available to the
     * given value.
     */
    void setDataFileUrls(QList<QUrl> dataFileUrls);
    
    /**
     * Defines where the document is valid or not.
     * 
     * The document is considered valid if at least one data file url is
     * specified (i.e. `dataFileUrls().length() > 0`) and the METAR URL
     * is not empty.
     */
    bool isValid() const;

private:
    QSharedDataPointer<VatsimStatusDocumentData> d;

}; /** @} */

}} /* namespace Vatsinator::Core */

Q_DECLARE_TYPEINFO(Vatsinator::Core::VatsimStatusDocument, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(Vatsinator::Core::VatsimStatusDocument)

#endif // CORE_VATSIMSTATUSDOCUMENT_H
