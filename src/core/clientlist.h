/*
 * clientlist.h
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

#ifndef CORE_CLIENTLIST_H
#define CORE_CLIENTLIST_H

#include "core/client.h"
#include "core/vtrcore_export.h"
#include <QtCore/QObject>
#include <QtCore/QSet>
#include <type_traits>

namespace Vatsinator { namespace Core {

/**
 * \ingroup Core
 * @{
 * 
 * The ClientList class is an aggregator for clients that tracks them
 * in a very simple way - when a client goes offline, it is automatically
 * removed.
 */
class VTRCORE_EXPORT ClientList : public QObject {
    Q_OBJECT

signals:
    /**
     * Emitted when a client is added.
     */
    void clientAdded(const Client* client);
    
    /**
     * Emitted when number of clients in the list changes.
     */
    void clientCountChanged();
    
public:
    /**
     * Creates a new, empty client list.
     */
    explicit ClientList(QObject* parent = nullptr);
    
    /**
     * Returns \c true if the list tracks the provided \c client already.
     */
    bool contains(const Client* client) const;
    
    /**
     * Returns number of clients in the list.
     */
    int count() const;
    
    /**
     * Gives direct access to the set of bound clients.
     */
    const QSet<const Client*>& asSet() const { return m_clients; }
    
    /**
     * Checks whether at least one client of the provided type is bound to
     * the airport object.
     */
    template<class T>
    bool hasClientOfType() const
    {
        return std::any_of(m_clients.begin(), m_clients.end(), [](auto it) -> bool {
            return qobject_cast<const T*>(it);
        });
    }
    
    /**
     * Returns the list of clients of the given type.
     */
    template<class T, typename = typename std::enable_if<std::is_base_of<Client, T>::value>::type>
    QList<const T*> clients() const
    {
        QList<const T*> clients;
        for (const Client* c: m_clients) {
            if (const T* t = qobject_cast<const T*>(c))
                clients << t;
        }
        return clients;
    }
    
public slots:
    /**
     * Adds the given \c client to the list.
     * 
     * Adds the client to the list if it does not yet exist, otherwise does
     * nothing. Starts tracking the provided client for state changes - if
     * the client goes offline, removes it from this list.
     */
    void add(const Client* client);
    
    /**
     * Removes any references to the \c client from the list.
     */
    void remove(const Client* client);
    
private slots:
    void removeClient(QObject* obj);
    
private:
    QSet<const Client*> m_clients;

}; /** @} */

}} /* namespace Vatsinator::Core */

#endif // CORE_CLIENTLIST_H
