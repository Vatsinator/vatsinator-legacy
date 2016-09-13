/*
 * map.cpp
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

#include "map.h"
#include "gui/airportitem.h"
#include "gui/firitem.h"
#include "gui/flightitem.h"
#include "gui/mapdrawer.h"
#include "gui/modelmatcher.h"
#include "core/resourcefile.h"
#include "core/pluginfinder.h"
#include <QtQuick>
#include <QtGui>

using namespace Vatsinator::Core;
using namespace Vatsinator::Gui;

namespace Vatsinator { namespace Imports {

Map::Map(QQuickItem* parent) :
    QQuickPaintedItem(parent),
    m_renderer(new MapRenderer(this)),
    m_scene(new MapScene(this))
{
    connect(m_renderer, &MapRenderer::centerChanged, this, &Map::centerChanged);
    connect(m_renderer, &MapRenderer::zoomChanged, this, &Map::zoomChanged);
    
    setFlag(QQuickItem::ItemHasContents);
    setFillColor(Qt::white);
    setOpaquePainting(true);
    
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 1))
    /* https://bugreports.qt.io/browse/QTBUG-52901 */
    setRenderTarget(FramebufferObject);
#endif
    
    connect(m_renderer, &MapRenderer::updated, this, &QQuickItem::update);
    
    m_renderer->setScene(m_scene);
    resetModelMappingsFile();
    
    MapDrawer* drawer = nullptr;
    auto plugins = PluginFinder::pluginsForIid(qobject_interface_iid<MapDrawer*>());
    if (plugins.length() > 0)
        drawer = qobject_cast<MapDrawer*>(plugins.first());
    
    m_renderer->setMapDrawer(drawer);
    
    connect(qApp, &QGuiApplication::aboutToQuit, this, &Map::saveMapState);
    connect(qApp, &QGuiApplication::applicationStateChanged, this, &Map::handleAppStateChanged);
    restoreMapState();
    
    connect(this, &QQuickItem::windowChanged, this, &Map::updateViewport);
}

Map::~Map()
{
    if (m_modelMatcher)
        delete m_modelMatcher;
}

void Map::paint(QPainter* painter)
{
    m_renderer->paint(painter);
}

void Map::setServerTracker(Core::ServerTracker* serverTracker)
{
    m_serverTracker = serverTracker;
    m_scene->track(serverTracker);
    emit serverTrackerChanged(m_serverTracker);
}

void Map::setModelMappingsFile(const QString& modelMappingsFile)
{
    if (modelMappingsFile != m_modelMappingsFile) {
        m_modelMappingsFile = modelMappingsFile;
        
//         ModelMatcher* mm = new ModelMatcher;
        /* TODO Implement this shit */
    }
}

void Map::resetModelMappingsFile()
{
    ResourceFile* mmFile = new ResourceFile("data/model.json", this);
    ModelMatcher* mm = new ModelMatcher;
    QJsonDocument document= QJsonDocument::fromJson(mmFile->data());
    mm->readFromJson(document);
    
    connect(mmFile, &ResourceFile::fileRead, [mmFile, mm]() {
        QJsonDocument document = QJsonDocument::fromJson(mmFile->data());
        mm->readFromJson(document);
    });
    
    m_scene->setModelMatcher(mm);
    
    if (m_modelMatcher)
        delete m_modelMatcher;
    
    m_modelMatcher = mm;
    m_modelMappingsFile = QString();
}

void Map::setControls(QQuickItem* controls)
{
    if (m_controls)
        disconnect(m_controls);
    
    bool c = connect(controls, SIGNAL(zoomUpdated(qreal)), this, SLOT(updateZoom(qreal)));
    if (!c)
        qWarning("Could not connect to %s::zoomUpdated(qreal)", controls->metaObject()->className());
    
    c = connect(controls, SIGNAL(positionUpdated(int, int)), this, SLOT(updatePosition(int,int)));
    if (!c)
        qWarning("Could not connect to %s::positionUpdated(int, int)", controls->metaObject()->className());
    
    c = connect(controls, SIGNAL(clicked(int, int)), this, SLOT(handleClicked(int,int)));
    if (!c)
        qWarning("Could not connect to %s::clicked(int, int)", controls->metaObject()->className());
    
    m_controls = controls;
    emit controlsChanged(m_controls);
}

void Map::updateZoom(qreal factor)
{
    m_renderer->setZoom(m_renderer->zoom() * factor);
}

void Map::updatePosition(int x, int y)
{
    WorldTransform t = m_renderer->transform();
    QPoint center = m_renderer->center() * t;
    QPoint diff(x, y);
    center -= diff;
    
    LonLat llcenter = t.map(center).bound();   
    m_renderer->setCenter(llcenter);
}

MapItem* Map::underPoint(int x, int y)
{
    Q_ASSERT(m_renderer);
    
    QPoint q(x, y);
    LonLat pos = m_renderer->transform().map(q);
    MapItem* item = m_renderer->scene()->nearest(pos.bound());
    if (item && item->contains(q, m_renderer->transform(pos.longitude())))
        return item;
    else
        return nullptr;
}

void Map::handleClicked(int x, int y)
{
    MapItem* clicked = underPoint(x, y);
    if (FlightItem* flightItem = qobject_cast<FlightItem*>(clicked)) {
        emit flightSelected(QVariant::fromValue<FlightItem*>(flightItem));
    } else if (AirportItem* airportItem = qobject_cast<AirportItem*>(clicked)) {
        emit airportSelected(QVariant::fromValue<AirportItem*>(airportItem));
    } else if (FirItem* firItem = qobject_cast<FirItem*>(clicked)) {
        emit firSelected(QVariant::fromValue<FirItem*>(firItem));
    }
}

void Map::saveMapState()
{
    QSettings s;
    s.setValue("map/center", this->center());
    s.setValue("map/zoom", this->zoom());
}

void Map::restoreMapState()
{
    QSettings s;
    QPointF center = s.value("map/center").toPointF();
    setCenter(center);
    qreal zoom = s.value("map/zoom", 1.0).toReal();
    setZoom(zoom);
}

void Map::handleAppStateChanged(Qt::ApplicationState state)
{
    if (state == Qt::ApplicationActive)
        restoreMapState();
    else
        saveMapState();
}

void Map::updateViewport(QQuickWindow* window)
{
    m_renderer->setViewport(window->size());
}

}} /* namespace Vatsinator::Imports */