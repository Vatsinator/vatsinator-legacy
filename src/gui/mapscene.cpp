/*
 * mapscene.cpp
 * Copyright (C) 2014  Michał Garapich <michal@garapich.pl>
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

#include "mapscene.h"
#include "core/tmalistreader.h"
#include "gui/airportitem.h"
#include "gui/firarea.h"
#include "gui/firitem.h"
#include "gui/flightitem.h"
#include "gui/pixmapprovider.h"
#include "gui/tmaarea.h"
#include "core/lonlat.h"
#include <QtCore>
#include <spatial/point_multimap.hpp>
#include <spatial/neighbor_iterator.hpp>
#include <spatial/region_iterator.hpp>
#include <spatial/idle_box_multimap.hpp>

using namespace Vatsinator::Core;

namespace Vatsinator { namespace Gui {

class MapScenePrivate {
    
    struct LonLatPointAccessor {
        qreal operator() (spatial::dimension_type dimension, const LonLat& p) const
        {
            switch (dimension) {
                case 0: return p.x();
                case 1: return p.y();
                default: Q_UNREACHABLE();
            }
        }
    };
    
    struct QRectFBoxAccessor {
        qreal operator() (spatial::dimension_type dim, const QRectF& rect) const
        {
            switch (dim) {
                case 0: return rect.left();
                case 1: return rect.top();
                case 2: return rect.right();
                case 3: return rect.bottom();
                default: Q_UNREACHABLE();
            }
        }
    };
    
    using ItemPointer = QSharedPointer<MapItem>;
    using ItemMap = spatial::point_multimap<2, LonLat, ItemPointer, spatial::accessor_less<LonLatPointAccessor, LonLat>>;
    
    using AreaPointer = QSharedPointer<MapArea>;
    using AreaMap = spatial::idle_box_multimap<4, QRectF, AreaPointer, spatial::accessor_less<QRectFBoxAccessor, QRectF>>;
    
public:
    MapScenePrivate(MapScene* scene) : m_scene(scene), m_tmaListReader(new TmaListReader) {}

    const MapItem* itemForFlight(const Pilot* flight)
    {
        if (!m_itemMapping.contains(flight))
            return nullptr;

        auto it = m_itemMapping.value(flight);
        return it->second.data();
    }
    
    void addFlight(const Pilot* flight)
    {
        if (flight->position().isNull()) {
            qFatal("Flight %s has an invalid position", qPrintable(flight->callsign()));
            return;
        }
        
        ItemPointer item(new FlightItem(const_cast<Pilot*>(flight), m_scene), &QObject::deleteLater);
        auto it = m_items.insert(std::make_pair(item->position(), item));
        m_itemMapping.insert(flight, it);
    }
    
    void removeFlight(const Pilot* flight)
    {
        Q_ASSERT(m_itemMapping.contains(flight));
        auto it = m_itemMapping.value(flight);
        m_items.erase(it);
        m_itemMapping.remove(flight);
    }
    
    void updateItem(const Pilot* flight)
    {
        Q_ASSERT(m_itemMapping.contains(flight));
        auto it = m_itemMapping.value(flight);
        QSharedPointer<FlightItem> ptr = qSharedPointerObjectCast<FlightItem>(it->second);
        Q_ASSERT(ptr);
        
        m_items.erase(it);
        m_itemMapping.remove(flight);
        
        addFlight(ptr->pilot());
    }
    
    void addAirport(const AirportObject* airport)
    {
        ItemPointer item(new AirportItem(const_cast<AirportObject*>(airport),
                                         m_scene),
                         &QObject::deleteLater);
        m_items.insert(std::make_pair(item->position(), item));
        
        AreaPointer area(new TmaArea(const_cast<AirportObject*>(airport),
                                     m_tmaListReader->coords(airport->icao()),
                                     m_scene),
                         &QObject::deleteLater);
        m_areas.insert(std::make_pair(area->boundingRect(), area));
    }
    
    void addFir(const FirObject* fir)
    {
        ItemPointer item(new FirItem(const_cast<FirObject*>(fir), m_scene), &QObject::deleteLater);
        m_items.insert(std::make_pair(item->position(), item));
        
        for (const Fir::Boundaries& boundary: fir->boundaries()) {
            AreaPointer area(new FirArea(const_cast<FirObject*>(fir), boundary, m_scene), &QObject::deleteLater);
            m_areas.insert(std::make_pair(area->boundingRect(), area));
        }
    }
    
    QList<const MapItem*> itemsInRect(const QRectF& rect) const
    {
        QList<const MapItem*> items;
    
        for (auto it = spatial::region_cbegin(m_items, rect.bottomLeft(), rect.topRight());
             it != spatial::region_cend(m_items, rect.bottomLeft(), rect.topRight()); ++it)
        {
            if (it->second->isVisible())
                items << it->second.data();
        }
    
        return items;
    }
    
    QList<const MapArea*> areasInRect(const QRectF& rect) const
    {
        auto pred = [&rect](spatial::dimension_type dim, spatial::dimension_type rank, const QRectF& area) -> spatial::relative_order {
            Q_UNUSED(rank);
            
            /* Checks whether rect contains any point of area */
            using namespace spatial;
            
            switch (dim) {
                case 0:
                case 2:
                    if (area.right() < rect.left())
                        return below;
                    else if (area.left() > rect.right())
                        return above;
                    else
                        return matching;
                    
                case 1:
                case 3:
                    if (area.bottom() > rect.top())
                        return above;
                    else if (area.top() < rect.bottom())
                        return below;
                    else
                        return matching;
                    
                default:
                    Q_UNREACHABLE();
            }
        };
        
        QList<const MapArea*> areas;
        
        for (auto it = spatial::region_cbegin(m_areas, pred);
             it != spatial::region_cend(m_areas, pred); ++it)
        {
            if (it->second->isVisible())
                areas << it->second.data();
        }
        
        
        return areas;
    }
    
    const MapItem* nearest(const LonLat& point) const
    {
        auto range = spatial::neighbor_range(m_items, point);
        auto it = std::find_if(range.first, range.second, [&point](auto it) {
            return it.second->isVisible();
        });
        
        return it == range.second ? nullptr : it->second.data();
    }

    QList<const MapItem*> nearest(int n, const LonLat& point) const
    {
        QList<const MapItem*> items;
        auto it = spatial::neighbor_cbegin(m_items, point);

        while (n > 0 && it != m_items.cend()) {
            items.append(it->second.data());
            --n;
            ++it;
        }

        return items;
    }
    
private:
    MapScene* m_scene;
    ItemMap m_items;
    AreaMap m_areas;
    QMap<const Pilot*, ItemMap::iterator> m_itemMapping;
    QScopedPointer<TmaListReader> m_tmaListReader;
    
};



MapScene::MapScene(QObject* parent) :
    QObject(parent),
    d(new MapScenePrivate(this)),
    m_pixmapProvider(new PixmapProvider(this)) {}

MapScene::~MapScene() {}

void MapScene::track(ServerTracker* serverTracker)
{
    connect(serverTracker, &ServerTracker::clientAdded, this, &MapScene::trackClient);
    connect(serverTracker, &ServerTracker::airportAdded, this, &MapScene::addAirport);
    
    auto firs = serverTracker->firObjects();
    for (auto f: firs)
        d->addFir(f);
}

QList<const MapItem*> MapScene::itemsInRect(const QRectF& rect) const
{
    Q_ASSERT(rect.bottomLeft().x() < rect.topRight().x());
    Q_ASSERT(rect.bottomLeft().y() < rect.topRight().y());
    
    return d->itemsInRect(rect);
}

QList<const MapArea*> MapScene::areasInRect(const QRectF& rect) const
{
    Q_ASSERT(rect.bottomLeft().x() < rect.topRight().x());
    Q_ASSERT(rect.bottomLeft().y() < rect.topRight().y());
    
    return d->areasInRect(rect);
}

const MapItem *MapScene::itemForFlight(const Pilot *flight)
{
    return d->itemForFlight(flight);
}

MapItem* MapScene::nearest(const LonLat& point)
{
    return const_cast<MapItem*>(d->nearest(point));
}

const MapItem* MapScene::nearest(const LonLat& point) const
{
    return d->nearest(point);
}

QList<const MapItem *> MapScene::nearest(int n, const LonLat &point) const
{
    return d->nearest(n, point);
}

void MapScene::setModelMatcher(const ModelMatcher* modelMatcher)
{
    m_modelMatcher = modelMatcher;
    m_pixmapProvider->setModelMatcher(m_modelMatcher);
}

void MapScene::setTrackedFlight(const Pilot *trackedFlight)
{
    if (m_trackedFlight != trackedFlight) {
        m_trackedFlight = trackedFlight;
        connect(trackedFlight, &Pilot::flightPhaseChanged, [this](Pilot::FlightPhase phase) {
            if (phase == Pilot::Arrived)
                this->cancelFlightTracking();
        });

        emit trackedFlightChanged(m_trackedFlight);
    }
}

void MapScene::addFlight(const Pilot* flight)
{
    d->addFlight(flight);
    connect(flight, &QObject::destroyed, this, &MapScene::removeClient, Qt::DirectConnection);
    connect(flight, &Client::positionChanged, this, &MapScene::updateFlight);
    
    emit updated();
}

void MapScene::removeFlight(const Pilot* flight)
{
    d->removeFlight(flight);
    emit updated();
}

void MapScene::addAirport(const AirportObject* airport)
{
    d->addAirport(airport);
}

void MapScene::trackClient(Client* client)
{
    Pilot* sp = qobject_cast<Pilot*>(client);
    if (sp)
        addFlight(sp);
}

void MapScene::updateFlight()
{
    Pilot* pilot = qobject_cast<Pilot*>(sender());
    Q_ASSERT(pilot);
    
    d->updateItem(pilot);
}

void MapScene::removeClient(QObject* obj)
{
    /* Note that we cannot qobject_cast obj to Pilot*, as obj is only
       a QObject - all its parent classes are already destroyed at this point */
    removeFlight(reinterpret_cast<Pilot*>(obj));
}

}} /* namespace Vatsinator::Gui */
