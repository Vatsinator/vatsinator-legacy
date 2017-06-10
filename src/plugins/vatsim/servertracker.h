#ifndef SERVERTRACKER_H
#define SERVERTRACKER_H

#include <core/servertracker.h>

namespace Vatsim {

class ServerTracker : public Vatsinator::Core::ServerTracker {
    Q_OBJECT

public:
    ServerTracker(QObject* parent = nullptr);
};

} // namespace Vatsim

#endif // SERVERTRACKER_H
