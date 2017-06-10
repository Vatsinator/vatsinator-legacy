#include "dataprovider.h"
#include <misc/resourcefile.h>
#include <QtCore>

using namespace Vatsinator::Core;

namespace VatsinatorDatabase {

DataProvider::DataProvider(QObject* parent) :
    Vatsinator::Misc::DataProvider(parent)
{
    loadData();
}

AirportModel *DataProvider::airports()
{
    return m_airports;
}

FirModel *DataProvider::firs()
{
    return m_firs;
}

void DataProvider::loadData()
{
    QElapsedTimer timer;
    timer.start();

    QFile f(":/data/airports.json");
    f.open(QIODevice::ReadOnly | QIODevice::Text);

    QJsonDocument document = QJsonDocument::fromJson(f.readAll());
    Q_ASSERT(document.isArray());
    QJsonArray array = document.array();

    AirportModel* airports = new AirportModel(this);
    for (const auto& it: qAsConst(array)) {
        QJsonObject o = it.toObject();
        if (!o.contains("icao") || !o.contains("longitude") || !o.contains("latitude")) {
            qWarning("airport.json corrupted: no ICAO code or position specified");
            continue;
        }

        Airport* a = new Airport(o.value("icao").toString(), this);
        a->setIata(o.value("iata").toString());
        a->setCity(o.value("city").toString());
        a->setCountry(o.value("country").toString());
        a->setName(o.value("name").toString());
        a->setAltitude(o.value("altitude").toInt());

        qreal lon = o.value("longitude").toDouble();
        qreal lat = o.value("latitude").toDouble();
        a->setPosition(LonLat(lon, lat));

        airports->insert(a);
    }

    f.close();
    qDebug("Read %d airports in %lld milliseconds", airports->count(), timer.elapsed());

    m_airports = airports;
    emit airportModelChanged(m_airports);
}

} // namespace VatsinatorDatabase
