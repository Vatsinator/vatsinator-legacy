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
class TileManager;
class QThreadPool;

/**
 * \ingroup Tiled-mad-drawer
 * @{
 */
class TileRenderer : public QObject {
    Q_OBJECT
    class TileRenderJob;
    
signals:
    void mapRendered(QRectF coords, QPixmap map);
    
public:
    explicit TileRenderer(QObject* parent = nullptr);

    QImage render(const QSize& viewport, const Vatsinator::Core::LonLat& center, qreal zoom);
    
private:
    quint32 zoomLevel(const Vatsinator::Gui::WorldTransform& transform);
    
private:
    TileManager* m_manager;
    QThreadPool* m_threadPool;
    
}; /** @} */

#endif // TILERENDERER_H
