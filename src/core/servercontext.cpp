#include "servercontext.h"

namespace Vatsinator { namespace Core {

ServerContext::ServerContext(QObject* parent) :
    QObject(parent),
    m_airports(new AirportListReader(this)),
    m_firs(new FirListReader(this)),
    m_uirs(new UirListReader(this)),
    m_airlines(new AirlineListReader(this))
{

}

AirportListReader *ServerContext::airports()
{
    return m_airports;
}

FirListReader *ServerContext::firs()
{
    return m_firs;
}

UirListReader *ServerContext::uirs()
{
    return m_uirs;
}

AirlineListReader *ServerContext::airlines()
{
    return m_airlines;
}

}} /* namespace Vatsinator::Core */
