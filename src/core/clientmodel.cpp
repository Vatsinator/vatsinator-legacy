#include "clientmodel.h"
#include "atc.h"
#include "atctracker.h"
#include "flighttracker.h"
#include "pilot.h"

namespace Vatsinator { namespace Core {

ClientModel::ClientModel(QObject* parent) : QObject(parent)
{

}

void ClientModel::addClient(Client* client)
{
    if (client->position().isNull()) {
        qWarning("%s has invalid position; not adding it this time", qPrintable(client->callsign()));
        client->deleteLater();
        return;
    }

    m_clients.insert(client->callsign(), client);

    if (Pilot* p = qobject_cast<Pilot*>(client)) {
        new FlightTracker(p, this);
    } else if (Atc* a = qobject_cast<Atc*>(client)) {
        new AtcTracker(a, this);
    }

    emit added(client);
}

Client *ClientModel::find(const QString &callsign)
{
    return m_clients.value(callsign, nullptr);
}

QList<Client *> ClientModel::asList() const
{
    return m_clients.values();
}

}} /* namespace Vatsinator::Core */
