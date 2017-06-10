#include "airportmodel.h"
#include <spatial/point_multimap.hpp>
#include <spatial/neighbor_iterator.hpp>


namespace Vatsinator { namespace Core {

class AirportModelPrivate {

    struct LonLatPointAccessor {
        qreal operator() (spatial::dimension_type dimension, const LonLat& p) const
        {
            switch (dimension) {
                case 0: return p.x();
                case 1: return p.y();
                default: Q_UNREACHABLE();
            }
        }
    };

    using AirportMap = spatial::point_multimap<2, LonLat, Airport*, spatial::accessor_less<LonLatPointAccessor, LonLat>>;

    AirportMap m_airports;

public:
    Airport* nearest(const LonLat& p) const
    {
        auto it = spatial::neighbor_cbegin(m_airports, p);
        return it == m_airports.end() ? nullptr : it->second;
    }

    Airport* findByIcao(const QString& icao) const
    {
        auto it = std::find_if(m_airports.begin(), m_airports.end(), [&icao](auto it) {
            return it.second->icao() == icao;
        });

        return it == m_airports.end() ? nullptr : it->second;
    }

    int size() const
    {
        return static_cast<int>(m_airports.size());
    }

    void insert(Airport* a) {
        m_airports.insert(std::make_pair(a->position(), a));
    }
};


AirportModel::AirportModel(QObject* parent) :
    QObject(parent),
    d(new AirportModelPrivate)
{

}

AirportModel::~AirportModel()
{

}

Airport *AirportModel::nearest(const LonLat &point) const
{
    return d->nearest(point);
}

Airport *AirportModel::findByIcao(const QString &icao) const
{
    return d->findByIcao(icao);
}

void AirportModel::insert(Airport *airport)
{
    d->insert(airport);
}

int AirportModel::count() const
{
    return d->size();
}

}}
