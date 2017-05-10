#include "marblemapdrawer.h"
#include "marble/AbstractFloatItem.h"
#include "marble/GeoPainter.h"
#include "marble/MarbleMap.h"
#include <QtCore>
#include <functional>

using namespace Vatsinator::Core;
using namespace Vatsinator::Gui;

namespace MarbleMap {

MarbleMapDrawer::MarbleMapDrawer(MapRenderer *renderer, QObject *parent) :
    QObject(parent),
    m_renderer(renderer),
    m_marbleMap(new Marble::MarbleMap)
{
    m_marbleMap->setMapThemeId("earth/openstreetmap/openstreetmap.dgml");
//    m_marbleMap->setMapThemeId("earth/googlemaps/googlemaps.dgml");
    m_marbleMap->setProjection(Marble::Projection::Mercator);
    m_marbleMap->setShowGrid(false);
    m_marbleMap->setShowCrosshairs(false);

    for (Marble::AbstractFloatItem* item: m_marbleMap->floatItems())
        item->setVisible(false);

    connect(renderer, &MapRenderer::centerChanged, this, &MarbleMapDrawer::setMarbleCenter);
    setMarbleCenter(renderer->center());

    connect(renderer, &MapRenderer::viewportChanged, this, &MarbleMapDrawer::setMarbleSize);
    setMarbleSize(renderer->viewport());

    connect(renderer, &MapRenderer::zoomChanged, this, &MarbleMapDrawer::setMarbleZoom);
    setMarbleZoom(renderer->zoom());
}

MarbleMapDrawer::~MarbleMapDrawer()
{

}

void MarbleMapDrawer::draw(WorldPainter* painter)
{
    QImage image(painter->transform().viewport(), QImage::Format_RGB32);
    image.fill(Qt::transparent);

    Marble::GeoPainter geoPainter(&image, m_marbleMap->viewport(), Marble::MapQuality::NormalQuality);
    m_marbleMap->paint(geoPainter, image.rect());

    painter->drawImage(painter->viewport(), image);
}

void MarbleMapDrawer::setMarbleCenter(const LonLat &center)
{
    m_marbleMap->setCenterLongitude(center.longitude());
    m_marbleMap->setCenterLatitude(center.latitude());
}

void MarbleMapDrawer::setMarbleSize(const QSize &size)
{
    m_marbleMap->setSize(size.width(), size.height());
    setMarbleZoom(m_renderer->zoom());
}

void MarbleMapDrawer::setMarbleZoom(qreal zoom)
{
    int factor = qMax(m_renderer->viewport().width(), m_renderer->viewport().height());
    m_marbleMap->setRadius(factor * zoom / 4);
}

} /* namespace MarbleMapDrawer */
