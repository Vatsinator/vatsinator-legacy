#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <misc/dataprovider.h>
#include <QtCore/QObject>

namespace VatsinatorDatabase {

class DataProvider : public Vatsinator::Misc::DataProvider {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.eu.vatsinator.v8rmisc.DataProvider" FILE "vatsinatordatabase.json")
    Q_INTERFACES(Vatsinator::Misc::DataProvider)

public:
    DataProvider(QObject* parent = nullptr);

    Vatsinator::Core::AirportModel* airports() override;
    Vatsinator::Core::FirModel* firs() override;

private:
    void loadData();

    Vatsinator::Core::AirportModel* m_airports { nullptr };
    Vatsinator::Core::FirModel* m_firs { nullptr };

};

} // namespace VatsinatorDatabase

#endif // DATAPROVIDER_H
