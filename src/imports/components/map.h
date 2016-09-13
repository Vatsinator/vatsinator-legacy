/*
 * map.h
 * Copyright (C) 2014 Michał Garapich <michal@garapich.pl>
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

#ifndef IMPORTS_MAP_H
#define IMPORTS_MAP_H

#include "core/lonlat.h"
#include "gui/mapitem.h"
#include "gui/maprenderer.h"
#include "gui/mapscene.h"
#include "core/servertracker.h"
#include <QQuickPaintedItem>

namespace Vatsinator { namespace Gui { class ModelMatcher; }}


namespace Vatsinator { namespace Imports {

/**
 * \ingroup Imports
 * @{
 * 
 * The Map class is an exposed to QML map surface. It provides useful methods
 * for the qml file, such as zoom and position handling.
 */
class Map : public QQuickPaintedItem {
    Q_OBJECT
    
    /**
     * The server tracked that is a data provider for this map.
     */
    Q_PROPERTY(Vatsinator::Core::ServerTracker* serverTracker READ serverTracker
               WRITE setServerTracker NOTIFY serverTrackerChanged)
    
    /**
     * File containing model mappings.
     * The default value uses \ref Vatsinator::Core::ResourceFile to keep
     * the mappings in-sync with Vatsinator repo.
     */
    Q_PROPERTY(QString modelMappingsFile READ modelMappingsFile
               WRITE setModelMappingsFile NOTIFY modelMappingsFileChanged
               RESET resetModelMappingsFile)
    
    /**
     * Instance of an item that makes it possible to move & zoom the map.
     * The item needs to provide the following signals:
     * * `zoomUpdated(var zoom)`
     * * `positionUpdated(var x, var y)`
     * * `clicked(var x, var y)`
     * 
     * \sa MapControls.
     */
    Q_PROPERTY(QQuickItem* controls READ controls WRITE setControls
               NOTIFY controlsChanged)
    
    /**
     * The current map position.
     */
    Q_PROPERTY(QPointF center READ center WRITE setCenter NOTIFY centerChanged)
    
    /**
     * The current zoom of the map.
     */
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    
signals:
    void zoomChanged(qreal zoom);
    void centerChanged(const QPointF& center);
    
    /**
     * Emitted when user clicks a flight. The type in the variant
     * is \c Vatsinator::Gui::FlightItem*.
     */
    void flightSelected(QVariant item);
    
    /**
     * Emitted when user clicks an airport. The type in the variant
     * is \c Vatsinator::Gui::AirportItem*.
     */
    void airportSelected(QVariant item);
    
    /**
     * Emitted when user clicks a FIR label. The type in the variant
     * is \c Vatsinator::Gui::FirItem*.
     */
    void firSelected(QVariant item);
    
    void serverTrackerChanged(Core::ServerTracker* serverTracker);
    void modelMappingsFileChanged(const QString& modelMappingsFile);
    void controlsChanged(QQuickItem* controls);
    
public:
    /**
     * Default constructor, passes \c parent to the \c QQuickItem.
     */
    Map(QQuickItem* parent = nullptr);
    
    /**
     * Destructor.
     */
    virtual ~Map();
    
    /**
     * \copydoc QQuickPaintedItem::paint()
     */
    void paint(QPainter* painter) override;
    
    /**
     * Gives direct access to the map renderer instance.
     * @{
     */
    Gui::MapRenderer* renderer() { return m_renderer; }
    const Gui::MapRenderer* renderer() const { return m_renderer; }
    /** @} */
    
    Core::ServerTracker* serverTracker() { return m_serverTracker; }
    const Core::ServerTracker* serverTracker() const { return m_serverTracker; }
    void setServerTracker(Core::ServerTracker* serverTracker);
    
    const QString& modelMappingsFile() const { return m_modelMappingsFile; }
    void setModelMappingsFile(const QString& modelMappingsFile);
    void resetModelMappingsFile();
    
    QQuickItem* controls() { return m_controls; }
    const QQuickItem* controls() const { return m_controls; }
    void setControls(QQuickItem* controls);
    
    QPointF center() const { return renderer()->center(); }
    void setCenter(const QPointF& center) { renderer()->setCenter(center); }
    
    qreal zoom() const { return renderer()->zoom(); }
    void setZoom(qreal zoom) { renderer()->setZoom(zoom); }
    
public slots:
    /**
     * Updates zoom by the given \c factor.
     */
    void updateZoom(qreal factor);
    
    /**
     * Moves map center by the [x, y] vector.
     */
    void updatePosition(int x, int y);
    
private:
    /**
     * Returns a MapItem that contains \c point.
     */
    Gui::MapItem* underPoint(int x, int y);
    
private slots:
    void handleClicked(int x, int y);
    void saveMapState();
    void restoreMapState();
    void handleAppStateChanged(Qt::ApplicationState state);
    void updateViewport(QQuickWindow* window);
    
private:
    Gui::MapRenderer* m_renderer = nullptr;
    Gui::MapItem* m_selectedItem = nullptr;
    Gui::MapScene* m_scene = nullptr;
    Gui::ModelMatcher* m_modelMatcher = nullptr;
    Core::ServerTracker* m_serverTracker = nullptr;
    QString m_modelMappingsFile;
    QQuickItem* m_controls = nullptr;
    
}; /** @} */

}} /* namespace Vatsinator::Imports */

#endif // IMPORTS_MAP_H
