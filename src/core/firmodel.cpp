#include "firmodel.h"

namespace Vatsinator { namespace Core {

FirModel::FirModel(QObject *parent) : QObject(parent)
{

}

Fir *FirModel::findByIcao(const QString &icao) const
{
    return m_firs.value(icao, nullptr);
}

}} /* namespace Vatsinator::Core */
