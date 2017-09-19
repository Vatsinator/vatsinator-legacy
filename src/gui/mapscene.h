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

#ifndef GUI_MAPSCENE_H
#define GUI_MAPSCENE_H

#include "core/pilot.h"
#include "core/servertracker.h"
#include "gui/maparea.h"
#include "gui/mapitem.h"
#include "gui/modelmatcher.h"
#include "gui/v8rgui_export.h"
#include <QtCore/QObject>
#include <QtCore/QRectF>

namespace Vatsinator { namespace Gui {

class MapScenePrivate; // hides k-d tree implementation details
class PixmapProvider;

/**
 * \ingroup Gui
 * @{
 * 
 * The MapScene class represents the main Vatsinator view.
 * 
 * The scene tracks a single server (\c trackServer) for clients changes
 * and keeps them in an efficient K-D tree.
 * 
 * \sa ServerTracker and MapRenderer.
 */
class V8RGUI_EXPORT MapScene : public QObject {
    Q_OBJECT

    /**
     * The tracked flight.
     */
    Q_PROPERTY(const Vatsinator::Core::Pilot* trackedFlight
               READ trackedFlight
               WRITE setTrackedFlight
               RESET cancelFlightTracking
               NOTIFY trackedFlightChanged)
    
signals:
    /**
     * Emitted when a change to the scene occurs.
     */
    void updated();

    void trackedFlightChanged(const Core::Pilot* tracekdFlight);

public:
    /**
     * Creates a new instance of MapScene class. Passes \c parent to the QObject's constructor.
     */
    explicit MapScene(QObject* parent = nullptr);
    
    /**
     * Destroys this MapScene.
     */
    virtual ~MapScene();
    
    /**
     * Tracks the server.
     * 
     * This method starts listening for new clients and adds them to the tree,
     * updates each of them on their position change and removes them, when
     * they go offline.
     */
    void track(Core::ServerTracker* serverTracker);
    
    /**
     * Returns a list of all items that exist within the provided boundaries.
     * 
     * \param rect The bounding rectangle used to select items.
     */
    QList<const MapItem*> itemsInRect(const QRectF& rect) const;
    
    /**
     * Returns a list of all areas that are visible inside the given \c rect.
     */
    QList<const MapArea*> areasInRect(const QRectF& rect) const;

    /**
     * Returns a corresponding \c FlightItem for the given \c flight.
     * If the scene does not contain \c flight, this method returns \c nullptr.
     *
     * \note The returned pointer can be safely casted to \c FlightItem*.
     */
    const MapItem* itemForFlight(const Core::Pilot* flight);
    
    /**
     * Returns a map item nearest to the given point.
     * @{
     */
    MapItem* nearest(const Core::LonLat& point);
    const MapItem* nearest(const Core::LonLat& point) const;
    /** @} */

    /**
     * Returns a list of \c n nearest items.
     */
    QList<const MapItem*> nearest(int n, const Core::LonLat& point) const;
    
    /**
     * Sets the working model matcher instance.
     * \note MapScene takes ownership over \c modelMatcher.
     */
    void setModelMatcher(ModelMatcher* modelMatcher);
    
    /**
     * Gives access to the PixmapProvider instance.
     */
    PixmapProvider* pixmapProvider() { return m_pixmapProvider; }
    
    /**
     * Gives access to the PixmapProvider instance.
     */
    const PixmapProvider* pixmapProvider() const { return m_pixmapProvider; }
    
    /**
     * Gives access to the currently running model matcher instance.
     * @{
     */
    const ModelMatcher* modelMatcher() const { return m_modelMatcher; }
    ModelMatcher* modelMatcher() { return m_modelMatcher; }
    /** @} */

    const Core::Pilot* trackedFlight() const { return m_trackedFlight; }
    void setTrackedFlight(const Core::Pilot* trackedFlight);
    void cancelFlightTracking() { setTrackedFlight(nullptr); }
    
public slots:
    /**
     * Adds a single flight to the scene.
     */
    void addFlight(const Core::Pilot* flight);
    
    /**
     * Removes the flight from the scene.
     */
    void removeFlight(const Core::Pilot* flight);
    
    /**
     * Starts tracking the provided airport.
     */
    void addAirport(const Core::Airport* airport);
    
private slots:
    /**
     * Performs a dynamic cast on the client and calls \ref trackFlight().
     */
    void trackClient(Core::Client* client);
    
    /**
     * Rebinds the flight item (which is sender), updating its position.
     */
    void updateFlight();
    
    /**
     * Removes the client from the scene.
     */
    void removeClient(QObject* obj);
    
private:
    QScopedPointer<MapScenePrivate> d;
    PixmapProvider* m_pixmapProvider = nullptr;
    ModelMatcher* m_modelMatcher = nullptr;
    const Core::Pilot* m_trackedFlight = nullptr;
    QMetaObject::Connection m_clientModelConnection;
    
}; /** @} */

}} /* namespace Vatsinator::Gui */

#endif // GUI_MAPSCENE_H
