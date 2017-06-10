#ifndef AIRPORTMODEL_H
#define AIRPORTMODEL_H

#include "core/airport.h"
#include "core/lonlat.h"
#include "core/vtrcore_export.h"
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>

namespace Vatsinator { namespace Core {

class AirportModelPrivate;

class VTRCORE_EXPORT AirportModel : public QObject {
    Q_OBJECT

public:
    AirportModel(QObject* parent = nullptr);

    virtual ~AirportModel();

    Airport* nearest(const LonLat& point) const;

    Airport* findByIcao(const QString& icao) const;

    void insert(Airport* airport);

    int count() const;

private:
    QScopedPointer<AirportModelPrivate> d;

};

}}

#endif // AIRPORTMODEL_H
