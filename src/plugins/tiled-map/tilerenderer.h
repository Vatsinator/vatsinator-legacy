/*
 * tilerenderer.h
 * Copyright (C) 2016 Michał Garapich <michal@garapich.pl>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifndef TILERENDERER_H
#define TILERENDERER_H

#include <core/lonlat.h>
#include <QObject>
#include <QPixmap>
#include <QRectF>

namespace Vatsinator { namespace Gui { class WorldTransform; }}
namespace Vatsinator { namespace Gui { class WorldPainter; }}

class QThreadPool;

namespace TiledMapDrawer {

class TileManager;
class Tile;

/**
 * \ingroup Tiled-map-drawer
 * @{
 */
class TileRenderer : public QObject {
    Q_OBJECT
    class TileRenderJob;
    
public:
    explicit TileRenderer(QObject* parent = nullptr);

    void render(Vatsinator::Gui::WorldPainter* painter);
    
private:
    QList<Tile> selectRows(const QList<Tile>& tiles, quint32 from, quint32 to);
    quint32 zoomLevel(const Vatsinator::Gui::WorldTransform& transform);
    
private:
    TileManager* m_manager;
    QThreadPool* m_threadPool;
    
}; /** @} */

} /* namespace TiledMapDrawer */

#endif // TILERENDERER_H
