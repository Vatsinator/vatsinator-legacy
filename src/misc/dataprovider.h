#ifndef MISC_DATAPROVIDER_H
#define MISC_DATAPROVIDER_H

#include "core/airportmodel.h"
#include "core/firmodel.h"
#include "misc/v8rmisc_export.h"
#include <QtCore/QObject>

namespace Vatsinator { namespace Misc {

class V8RMISC_EXPORT DataProvider : public QObject {
    Q_OBJECT

signals:
    void airportModelChanged(Core::AirportModel* newModel);
    void firModelChanged(Core::FirModel* newModel);

public:
    DataProvider(QObject* parent = nullptr);

    virtual Core::AirportModel* airports() = 0;
    virtual Core::FirModel* firs() = 0;

};

}} /* namespace Vatsinator::Misc */

Q_DECLARE_INTERFACE(Vatsinator::Misc::DataProvider, "org.eu.vatsinator.v8rmisc.DataProvider")

#endif // MISC_DATAPROVIDER_H
