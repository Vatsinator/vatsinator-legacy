/*
 * vatsimdatadocument.h
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

#ifndef CORE_VATSIMDATADOCUMENT_H
#define CORE_VATSIMDATADOCUMENT_H

#include "core/clientdata.h"
#include "core/v8rcore_export.h"
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QMetaType>
#include <QtCore/QSharedDataPointer>

namespace Vatsinator { namespace Core {

class VatsimDataDocumentData;

/**
 * \ingroup Core
 * @{
 * 
 * The VatsimDataDocument class 
 */
class V8RCORE_EXPORT VatsimDataDocument {
public:
    /**
     * Creates an empty (and invalid) document.
     */
    VatsimDataDocument();
    
    /**
     * Creates a document that is a copy of \c other.
     */
    VatsimDataDocument(const VatsimDataDocument& other);
    
    /**
     * Assigns \c other to this one.
     */
    VatsimDataDocument& operator =(const VatsimDataDocument& other);
    
    virtual ~VatsimDataDocument();
    
    int connectedClients() const;
    void setConnectedClients(int connectedClients);
    QDateTime update() const;
    void setUpdate(const QDateTime& update);
    int reload() const;
    void setReload(int reload);
    const QList<ClientData>& clients() const;
    void addClient(ClientData client);
    const QList<ClientData>& prefiled() const;
    void addPrefiled(ClientData client);

private:
    QSharedDataPointer<VatsimDataDocumentData> d;

}; /** @} */

}} /* namespace Vatsinator::Core */

Q_DECLARE_TYPEINFO(Vatsinator::Core::VatsimDataDocument, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(Vatsinator::Core::VatsimDataDocument)

#endif // CORE_VATSIMDATADOCUMENT_H
