#ifndef CORE_CLIENTMODEL_H
#define CORE_CLIENTMODEL_H

#include "core/client.h"
#include "core/vtrcore_export.h"
#include <QtCore/QMap>
#include <QtCore/QObject>

namespace Vatsinator { namespace Core {

class VTRCORE_EXPORT ClientModel : public QObject {
    Q_OBJECT

signals:
    void added(Client* client);

public:
    ClientModel(QObject* parent = nullptr);

    virtual void addClient(Client* client);

    virtual Client* find(const QString& callsign);

    QList<Client*> asList() const;

private:
    QMap<QString, Client*> m_clients;

};

}} /* namespace Vatsinator::Core */

#endif // CORE_CLIENTMODEL_H
