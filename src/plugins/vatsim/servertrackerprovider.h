#ifndef SERVERTRACKERPROVIDER_H
#define SERVERTRACKERPROVIDER_H

#include <misc/servertrackerprovider.h>
#include <QObject>

namespace Vatsim {

class ServerTrackerProvider : public QObject, public Vatsinator::Misc::ServerTrackerProvider {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.eu.vatsinator.v8rmisc.ServerTrackerProvider" FILE "vatsim.json")
    Q_INTERFACES(Vatsinator::Misc::ServerTrackerProvider)

public:
    explicit ServerTrackerProvider(QObject* parent = nullptr);

    Vatsinator::Core::ServerTracker* serverTracker() override;

};

} // namespace Vatsim

#endif // SERVERTRACKERPROVIDER_H
