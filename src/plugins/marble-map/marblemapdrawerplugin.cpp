#include "marblemapdrawerplugin.h"
#include "marblemapdrawer.h"

namespace MarbleMapDrawer {

MarbleMapDrawerPlugin::MarbleMapDrawerPlugin(QObject *parent) : QObject(parent)
{

}

Vatsinator::Gui::MapDrawer *MarbleMapDrawerPlugin::create(Vatsinator::Gui::MapRenderer *renderer)
{
    return new MarbleMapDrawer(renderer, this);
}

} /* namespace MarbleMapDrawer */
