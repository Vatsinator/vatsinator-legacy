#ifndef MARBLEMAPDRAWERPLUGIN_H
#define MARBLEMAPDRAWERPLUGIN_H

#include "gui/mapdrawerplugin.h"
#include <QObject>

/**
 * \defgroup Marble-map-drawer
 * A world map drawer that uses Marble.
 */

namespace MarbleMapDrawer {

class MarbleMapDrawerPlugin : public QObject, public Vatsinator::Gui::MapDrawerPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.eu.vatsinator.vtrgui.MapDrawerPlugin" FILE "marblemapdrawer.json")
    Q_INTERFACES(Vatsinator::Gui::MapDrawerPlugin)

public:
    MarbleMapDrawerPlugin(QObject* parent = nullptr);

    Vatsinator::Gui::MapDrawer* create(Vatsinator::Gui::MapRenderer* renderer) override;

};

} /* namespace MarbleMapDrawer */

#endif // MARBLEMAPDRAWERPLUGIN_H
