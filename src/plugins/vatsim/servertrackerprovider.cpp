#include "servertrackerprovider.h"
#include "servertracker.h"

namespace Vatsim {

ServerTrackerProvider::ServerTrackerProvider(QObject* parent) : QObject(parent)
{

}

Vatsinator::Core::ServerTracker *ServerTrackerProvider::serverTracker()
{
    static ServerTracker* st = new ServerTracker(parent());
    return st;
}

} // namespace Vatsim
