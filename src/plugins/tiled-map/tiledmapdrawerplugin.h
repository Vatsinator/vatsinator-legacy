#ifndef TILEDMAPDRAWERPLUGIN_H
#define TILEDMAPDRAWERPLUGIN_H

#include "gui/mapdrawerplugin.h"
#include <QObject>

/**
 * \defgroup Tiled-map-drawer
 * A tile-based world map drawer.
 */

namespace TiledMapDrawer {

class TiledMapDrawerPlugin : public QObject, public Vatsinator::Gui::MapDrawerPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.eu.vatsinator.v8rgui.MapDrawerPlugin" FILE "tiledmapdrawer.json")
    Q_INTERFACES(Vatsinator::Gui::MapDrawerPlugin)

public:
    TiledMapDrawerPlugin(QObject* parent = nullptr);

    Vatsinator::Gui::MapDrawer* create(Vatsinator::Gui::MapRenderer* renderer) override;

};

} /* namespace TiledMapDrawer */

#endif // TILEDMAPDRAWERPLUGIN_H
