#ifndef CORE_SERVERCONTEXT_H
#define CORE_SERVERCONTEXT_H

#include "core/airlinelistreader.h"
#include "core/airportlistreader.h"
#include "core/firlistreader.h"
#include "core/uirlistreader.h"
#include "core/vtrcore_export.h"
#include <QtCore/QObject>

namespace Vatsinator { namespace Core {

class VTRCORE_EXPORT ServerContext : public QObject {
    Q_OBJECT

public:
    ServerContext(QObject* parent = nullptr);

    virtual AirportListReader* airports();
    virtual FirListReader* firs();
    virtual UirListReader* uirs();
    virtual AirlineListReader* airlines();

private:
    AirportListReader* m_airports;
    FirListReader* m_firs;
    UirListReader* m_uirs;
    AirlineListReader* m_airlines;

};

}} /* namespace Vatsinator::Core */

#endif // CORE_SERVERCONTEXT_H
