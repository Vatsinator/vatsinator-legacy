/*
 * mapscene.h
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

#ifndef MAPSCENE_H
#define MAPSCENE_H

#include <functional>
#include <QObject>
#include <QList>
#include <QColor>
#include <QRectF>
#include <spatial/bits/spatial_check_concept.hpp>
#include <spatial/point_multimap.hpp>
#include <spatial/idle_box_multimap.hpp>
#include "vatsimdata/lonlat.h"

class QAbstractAnimation;
class QRectF;
class QSignalMapper;
class AirportItem;
class Controller;
class Fir;
class FirItem;
class FlightItem;
class MapArea;
class MapItem;
class Pilot;
class PixmapProvider;
class MapRenderer;

/**
 * The MapScene class is responsible for managing all the map items, moving
 * the map around (except situations when user moves it using mouse) and
 * zooming it appropriately.
 *
 * \sa MapRenderer.
 */
class MapScene : public QObject {
    Q_OBJECT
    
public:
    /**
     * The settings struct provides fast access to map-related settings.
     * The MapScene class makes sure these settings are always up-to-date.
     * SettingsManager::get() calls are pretty expensive, as there is always
     * some casting from QVariant involved, thus this struct keeps everything
     * here, updating it only when needed.
     */
    struct MapSettings {
        
        struct {
            bool airports_layer;
            bool airport_labels;
            bool pilots_layer;
            bool staffed_firs;
            bool unstaffed_firs;
            bool empty_airports;
            
            struct {
                bool always;
                bool airport_related;
                bool when_hovered;
            } pilot_labels;
        } view;
    };
    
signals:
    /**
     * Emitted when user wants to track the flight.
     * If the pointer is nullptr, it means that user has just cancelled
     * flight tracking.
     */
    void flightTracked(const Pilot* pilot);
    
public:
    /**
     * Constructs new MapScene. Passes \c parent to the QObject's constructor.
     */
    explicit MapScene(QObject* parent);
    
    /**
     * Adds a new area to the scene.
     */
    void addArea(MapArea* area);
    
    /**
     * Removes the given area from the scene.
     */
   void removeArea(MapArea* area);
    
    /**
     * Marks the specified pilot as tracked one.
     * Tracked flight will be always displayed in the center of the map.
     * It is used in order to keep track of the flight when user hides
     * the main window. When this function is called, the flightTracked()
     * signal will be emitted.
     * If any flight was tracked before, it will be switched to _pilot_.
     * The flight tracking can be cancelled by calling cancelFlightTracking()
     * function.
     *
     * \param pilot The pilot to be tracked on the map.
     */
    void trackFlight(const Pilot* pilot);
    
    /**
     * Stops tracking any flight.
     *
     * This is the same as calling \c MapScene::trackFlight() with \c nullptr.
     */
    inline void cancelFlightTracking()
    {
        trackFlight(nullptr);
    }
    
    /**
     * Executes the given function for each item found inside the given \c rect.
     *
     * \param rect The bounding rectangle for items.
     * \param function The function to execute on every item found.
     *
     */
    void inRect(const QRectF& rect, std::function<void(const MapItem*)> function) const;
    
    /**
     * Returns a list of all items that exist within the provided boundaries.
     * 
     * \param rect The bounding rectangle used to select items.
     */
    QList<const MapItem*> itemsInRect(const QRectF& rect) const;
    
    /**
     * Executes the provided function for each area that is visible in the given
     * \c rect.
     */
    void inRect(const QRectF& rect, std::function<void(const MapArea*)> function) const;
    
    /**
     * Finds nearest item to the given _point_.
     *
     * \note This item never returns _nullptr_.
     */
    const MapItem* nearest(const LonLat& point) const;
    
    /**
     * Executes the given function on maximum _max_ nearest items.
     *
     * \param point The reference point.
     * \param max Maximum number of items.
     * \param function The function to be executed on every item.
     */
    void nearTo(const LonLat& point, int max, std::function<void(const MapItem*)> function) const;
    
    /**
     * Gets the running instance of MapRenderer.
     */
    inline MapRenderer* renderer()
    {
        return __renderer;
    }
    
    /**
     * Gets the currently tracked flight.
     */
    inline const Pilot* trackedFlight() const
    {
        return __trackedFlight;
    }
    
    /**
     * Gives access to all settings in a pretty fast way.
     */
    inline const MapSettings& settings() const
    {
        return __settings;
    }
    
    /**
     * Gives access to the pixmap provider.
     */
    inline PixmapProvider* pixmapProvider()
    {
        return __pixmapProvider;
    }
    
    /**
     * Returns true if the map is in the middle of an animation.
     */
    inline bool isAnimating() const
    {
        return __animation != nullptr;
    }
    
    virtual ~MapScene() = default;
    
public slots:
    /**
     * Moves the map smoothly to the given point.
     *
     * \param target The target point.
     */
    void moveTo(const LonLat& target);
    
    /**
     * Zooms smoothly to the given value.
     *
     * \param zoom The target zoom value.
     */
    void zoomTo(qreal zoom);
    
    /**
     * Aborts the currently running animation.
     * If no animation is being played, this function has no effect.
     */
    void abortAnimation();
    
private:
    void __addFlightItem(const Pilot*);
    
private slots:
    void __setupItems();
    void __updateItems();
    
    /**
     * This slot is connected to every Pilot's destroyed() signal.
     */
    void __removeFlightItem(QObject* object);
    
    /**
     * Update flight position in the tree.
     */
    void __updateFlightItem();
    
    /**
     * Updates locally-kept settings.
     */
    void __updateSettings();
    
private:
    MapRenderer* __renderer;
    
    struct LonLatPointAccessor {
        qreal operator() (spatial::dimension_type dim, const LonLat& p) const
        {
            switch (dim) {
                case 0:
                    return p.x();
                    
                case 1:
                    return p.y();
                    
                default:
                    Q_UNREACHABLE();
            }
        }
    };
    
    struct QRectFBoxAccessor {
        qreal operator() (spatial::dimension_type dim, const QRectF& rect) const
        {
            switch (dim) {
                case 0:
                    return rect.topLeft().x();
                case 1:
                    return rect.topLeft().y();
                case 2:
                    return rect.bottomRight().x();
                case 3:
                    return rect.bottomRight().y();
                default:
                    Q_UNREACHABLE();
            }
        }
    };
    
    /**
     * This map keeps FlightItems, AirportItems and FirItems.
     */
    spatial::point_multimap<2, LonLat, const MapItem*, spatial::accessor_less<LonLatPointAccessor, LonLat>> __items;
    
    /**
     * This map keeps FirAreas.
     */
    spatial::idle_box_multimap<4, QRectF, const MapArea*, spatial::accessor_less<QRectFBoxAccessor, QRectF>> __areas;
    
    const Pilot* __trackedFlight;
    QAbstractAnimation* __animation;
    
    PixmapProvider* __pixmapProvider;
    MapSettings __settings;
    
    QSignalMapper* __flightsMapper; /**< Maps Pilots to its iterators in the spatial map */
    
};

#endif // MAPSCENE_H
