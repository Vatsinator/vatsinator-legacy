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

#include <algorithm>
#include <utility>
#include <QtCore>
#include <spatial/point_multimap.hpp>
#include <spatial/neighbor_iterator.hpp>
#include <spatial/region_iterator.hpp>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "storage/settingsmanager.h"
#include "ui/map/airportitem.h"
#include "ui/map/firarea.h"
#include "ui/map/firitem.h"
#include "ui/map/flightitem.h"
#include "ui/map/maprenderer.h"
#include "ui/map/pixmapprovider.h"
#include "vatsimdata/controller.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "mapscene.h"

MapScene::MapScene(QObject* parent) :
    QObject(parent),
    __renderer(qobject_cast<MapRenderer*>(parent)),
    __trackedFlight(nullptr),
    __animation(nullptr),
    __pixmapProvider(new PixmapProvider(this)),
    __flightsMapper(new QSignalMapper(this))
{
    Q_ASSERT(__renderer);
    
    connect(__flightsMapper, static_cast<void (QSignalMapper::*)(QObject*)>(&QSignalMapper::mapped), this, &MapScene::__removeFlightItem);
    connect(vApp()->vatsimDataHandler(), &VatsimDataHandler::vatsimDataUpdated, this, &MapScene::__updateItems);
    connect(vApp()->vatsimDataHandler(), &VatsimDataHandler::initialized, this, &MapScene::__setupItems);
    connect(vApp()->settingsManager(), &SettingsManager::settingsChanged, this, &MapScene::__updateSettings);
    
    connect(this, &MapScene::flightTracked, [this](const Pilot * p) {
        if (p)
            moveTo(p->position());
    });
    
    __updateSettings();
}

void
MapScene::addArea(MapArea* area)
{
    __areas.insert(std::make_pair(area->boundingRect(), area));
}

void
MapScene::removeArea(MapArea* area)
{
    auto it = std::find_if(__areas.begin(), __areas.end(), [area](const auto& it) {
        return it.second == area;
    });
    
    if (it != __areas.end())
        __areas.erase(it);
}

void
MapScene::trackFlight(const Pilot* pilot)
{
    __trackedFlight = pilot;
    emit flightTracked(__trackedFlight);
}

void
MapScene::inRect(const QRectF& rect, std::function<void(const MapItem*)> function) const
{
    Q_ASSERT(rect.bottomLeft().x() < rect.topRight().x());
    Q_ASSERT(rect.bottomLeft().y() < rect.topRight().y());
    
    std::for_each(spatial::region_cbegin(__items, rect.bottomLeft(), rect.topRight()),
                  spatial::region_cend(__items, rect.bottomLeft(), rect.topRight()),
                  [&function](auto it) {
                      if (it.second->isVisible())
                          function(it.second);
                  });
}

QList<const MapItem*>
MapScene::itemsInRect(const QRectF& rect) const
{
    Q_ASSERT(rect.bottomLeft().x() < rect.topRight().x());
    Q_ASSERT(rect.bottomLeft().y() < rect.topRight().y());
    
    QList<const MapItem*> items;
    
    std::for_each(spatial::region_cbegin(__items, rect.bottomLeft(), rect.topRight()),
                  spatial::region_cend(__items, rect.bottomLeft(), rect.topRight()),
                  [&items](auto it) {
                      if (it.second->isVisible())
                          items << it.second;
                  });
    
    return items;
}

void
MapScene::inRect(const QRectF& rect, std::function<void(const MapArea*)> function) const
{
    Q_ASSERT(rect.bottomLeft().x() < rect.topRight().x());
    Q_ASSERT(rect.bottomLeft().y() < rect.topRight().y());
    
    auto pred = [&rect](spatial::dimension_type dim, spatial::dimension_type rank, const QRectF& area) -> spatial::relative_order {
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
        
        Q_UNUSED(rank);
    };
    
    std::for_each(spatial::region_cbegin(__areas, pred), spatial::region_cend(__areas, pred),
                  [&function](auto it) {
                      if (it.second->isVisible())
                          function(it.second);
                  });
}

const MapItem*
MapScene::nearest(const LonLat& point) const
{
    auto it = spatial::neighbor_begin(__items, point);
    
    if (it == __items.end())
        return nullptr;
        
    while (!it->second->isVisible()) {
        ++it;
        
        if (it == __items.end())
            return nullptr;
    }
    
    return it->second;
}

void
MapScene::nearTo(const LonLat& point, int max, std::function<void(const MapItem*)> function) const
{
    auto it = spatial::neighbor_begin(__items, point);
    int c = 0;
    
    while (c < max && it != __items.end()) {
        if (it->second->isVisible()) {
            function(it->second);
            c += 1;
        }
        
        ++ it;
    }
}

void
MapScene::moveTo(const LonLat& target)
{
    abortAnimation();
    
    QPropertyAnimation* animation = new QPropertyAnimation(__renderer, "center");
    animation->setDuration(500);
    animation->setEndValue(QVariant::fromValue<LonLat>(target));
    animation->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
    animation->start();
    
    __animation = animation;
    connect(animation, &QPropertyAnimation::finished, [this]() {
        __animation->deleteLater();
        __animation = nullptr;
    });
}

void
MapScene::zoomTo(qreal zoom)
{
    abortAnimation();
    
    QPropertyAnimation* animation = new QPropertyAnimation(__renderer, "zoom");
    animation->setDuration(200);
    animation->setEndValue(zoom);
    animation->setEasingCurve(QEasingCurve(QEasingCurve::OutQuad));
    animation->start();
    
    __animation = animation;
    connect(animation, &QPropertyAnimation::finished, [this]() {
        __animation->deleteLater();
        __animation = nullptr;
        /* Forces the MapRenderer::zoomChanged() signal emmition */
        __renderer->setZoom(__renderer->zoom());
    });
}

void
MapScene::abortAnimation()
{
    if (__animation) {
        __animation->stop();
        __animation->deleteLater();
        __animation = nullptr;
    }
}

void
MapScene::__addFlightItem(const Pilot* pilot)
{
    Q_ASSERT(!pilot->position().isNull());
    
    FlightItem* item = new FlightItem(pilot, this);
    Q_ASSERT(item->position() == pilot->position());
    __items.insert(std::make_pair(item->position(), item));
    
    __flightsMapper->setMapping(const_cast<Pilot*>(pilot), item);
    connect(pilot, SIGNAL(invalid()), __flightsMapper, SLOT(map()));
    connect(pilot, &Pilot::positionChanged, this, &MapScene::__updateFlightItem);
}

void
MapScene::__setupItems()
{
    for (const Airport* a : vApp()->vatsimDataHandler()->airports()) {
        AirportItem* item = new AirportItem(a, this);
        Q_ASSERT(item->position() == a->position());
        __items.insert(std::make_pair(item->position(), item));
    }
    
    for (const Fir* f : vApp()->vatsimDataHandler()->firs()) {
        if (f->data()->header.textPosition.x != 0.0 && f->data()->header.textPosition.y != 0.0) {
            FirItem* item = new FirItem(f, this);
            __items.insert(std::make_pair(item->position(), item));
            FirArea* area = new FirArea(f, this);
            __areas.insert(std::make_pair(area->boundingRect(), area));
        }
    }
    
//     for (const Uir* u : vApp()->vatsimDataHandler()->uirs())
//         __uirItems << new UirItem(u, this);
    
    for (auto c : vApp()->vatsimDataHandler()->clients()) {
        if (Pilot* p = qobject_cast<Pilot*>(c)) {
            __addFlightItem(p);
        }
    }
}

void
MapScene::__updateItems()
{
    for (Client* c : vApp()->vatsimDataHandler()->newClients())
        if (Pilot* p = qobject_cast<Pilot*>(c)) {
            __addFlightItem(p);
        }
}

void
MapScene::__removeFlightItem(QObject* object)
{
    FlightItem* item = qobject_cast<FlightItem*>(object);
    Q_ASSERT(item);
    
    auto it = std::find_if(spatial::equal_begin(__items, item->data()->position()),
                           spatial::equal_end(__items, item->data()->position()),
                           [item](auto it) {
                               return item == it.second;
                           });
    
    item->deleteLater();
    __items.erase(it);
}

void
MapScene::__updateFlightItem()
{
    Q_ASSERT(sender());
    
    /*
     * As there is no rebalance() method, we need to remove the corresponding item
     * and insert it back again, with the updated position.
     */
    Pilot* p = qobject_cast<Pilot*>(sender());
    Q_ASSERT(p);
    
    auto it = std::find_if(spatial::equal_begin(__items, p->oldPosition()),
                           spatial::equal_end(__items, p->oldPosition()),
                           [p](auto it) {
                               const FlightItem* citem = qobject_cast<const FlightItem*>(it.second);
                               return citem && citem->data() == p;
                           });
    
    Q_ASSERT(it != __items.end());
    
    const MapItem* item = it->second;
    Q_ASSERT(item);
    __items.erase(it);
    __items.insert(std::make_pair(p->position(), item));
}

void
MapScene::__updateSettings()
{
    __settings.view.airports_layer = SM::get("view.airports_layer").toBool();
    __settings.view.airport_labels = SM::get("view.airport_labels").toBool();
    __settings.view.pilots_layer = SM::get("view.pilots_layer").toBool();
    __settings.view.staffed_firs = SM::get("view.staffed_firs").toBool();
    __settings.view.unstaffed_firs = SM::get("view.unstaffed_firs").toBool();
    __settings.view.empty_airports = SM::get("view.empty_airports").toBool();
    __settings.view.pilot_labels.always = SM::get("view.pilot_labels.always").toBool();
    __settings.view.pilot_labels.airport_related = SM::get("view.pilot_labels.airport_related").toBool();
    __settings.view.pilot_labels.when_hovered = SM::get("view.pilot_labels.when_hovered").toBool();
}
