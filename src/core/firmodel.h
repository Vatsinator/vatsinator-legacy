#ifndef FIRMODEL_H
#define FIRMODEL_H

#include "core/fir.h"
#include "core/v8rcore_export.h"
#include <QtCore/QMap>
#include <QtCore/QObject>

namespace Vatsinator { namespace Core {

class V8RCORE_EXPORT FirModel : public QObject {
    Q_OBJECT

public:
    FirModel(QObject* parent = nullptr);

    Fir* findByIcao(const QString& icao) const;

private:
    QMap<QString, Fir*> m_firs;

};

}} /* namespace Vatsinator::Core */

#endif // FIRMODEL_H
