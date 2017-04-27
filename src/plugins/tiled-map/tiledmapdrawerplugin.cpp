#include "tiledmapdrawerplugin.h"
#include "tiledmapdrawer.h"

namespace TiledMapDrawer {

TiledMapDrawerPlugin::TiledMapDrawerPlugin(QObject *parent) : QObject(parent)
{

}

Vatsinator::Gui::MapDrawer* TiledMapDrawerPlugin::create(Vatsinator::Gui::MapRenderer *renderer)
{
    return new TiledMapDrawer(renderer);
}

} /* namespace TiledMapDrawer */
