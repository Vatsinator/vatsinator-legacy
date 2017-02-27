/*
 * mapwidget.cpp
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
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

#include "mapwidget.h"
#include "widgetsprivate.h"
#include "core/atc.h"
#include "core/pilot.h"
#include "gui/airportitem.h"
#include "gui/firitem.h"
#include "gui/flightitem.h"
#include "gui/mapscene.h"
#include <QtWidgets>
#include <functional>

using namespace Vatsinator::Core;
using namespace Vatsinator::Gui;

namespace Vatsinator { namespace Widgets {

/**
 * Simplifies mouse handling.
 */
class MouseHelper {
public:
    void update(const QPoint& widgetPosition)
    {
        m_widgetPosition = widgetPosition;
    }
    
    const QPoint& widgetPosition() const { return m_widgetPosition; }
    
    bool isDown() const { return m_down; }
    void setDown(bool down) { m_down = down; }
    
    const QPoint& lastClick() const { return m_lastClick; }
    void setLastClick(const QPoint& lastClick) { m_lastClick = lastClick; }
    
private:
    QPoint m_widgetPosition = QPoint();
    bool m_down = false;
    QPoint m_lastClick = QPoint();
};


MapWidget::MapWidget(QWidget* parent) :
    QWidget(parent),
    m_mouse(new MouseHelper)
{
    setAttribute(Qt::WA_AcceptTouchEvents);
    setAttribute(Qt::WA_TouchPadAcceptSingleTouchEvents);
    grabGesture (Qt::PinchGesture);
    setMouseTracking(true);
}

MapWidget::~MapWidget() {}

bool MapWidget::event(QEvent* event)
{
    switch (event->type()) {
        case QEvent::ToolTip: {
            QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
            const MapItem* item = underPoint(helpEvent->pos());
            
            if (item) {
                QString text = makeToolTip(item);
                QToolTip::showText(helpEvent->globalPos(), text);
            } else {
                QToolTip::hideText();
                event->ignore();
            }
            
            return true;
        }
        
        default:
            return QWidget::event(event);
    }
}

void MapWidget::setRenderer(MapRenderer* renderer)
{
    m_renderer = renderer;
    connect(m_renderer, &MapRenderer::updated, this, static_cast<void (MapWidget::*)()>(&MapWidget::update));
    m_renderer->setViewport(this->size());
}

void MapWidget::paintEvent(QPaintEvent* event)
{
    if (!renderer()) {
        QWidget::paintEvent(event);
        return;
    }
    
    QSet<const MapItem*> selected;
    if (!m_mouse->isDown()) {
        const MapItem* item = underPoint(m_mouse->widgetPosition());
        if (item) {
            setCursor(QCursor(Qt::PointingHandCursor));
            selected.insert(item);
        } else {
            setCursor(QCursor(Qt::ArrowCursor));
        }
    }

    if (renderer()->scene()->trackedFlight())
        selected.insert(renderer()->scene()->itemForFlight(renderer()->scene()->trackedFlight()));
    
    m_renderer->paint(this, selected);
    
    Q_UNUSED(event);
}

void MapWidget::resizeEvent(QResizeEvent* event)
{
    if (renderer())
        renderer()->setViewport(event->size());
}

void MapWidget::wheelEvent(QWheelEvent* event)
{   
    if (renderer()) {
        if (event->phase() == Qt::NoScrollPhase || event->phase() == Qt::ScrollUpdate) {
            int steps = event->angleDelta().y() / 120;
            renderer()->zoomTo(m_renderer->zoom() * qPow(2, steps));
        }
        event->accept();
    } else {
        event->ignore();
    }
}

void MapWidget::mousePressEvent(QMouseEvent* event)
{
    m_mouse->update(event->pos());
    m_mouse->setDown(true);
    QToolTip::hideText();
    
    if (event->button() & Qt::LeftButton)
        m_mouse->setLastClick(event->pos());
    
    event->accept();
    update();
}

void MapWidget::mouseReleaseEvent(QMouseEvent* event)
{
    m_mouse->update(event->pos());
    m_mouse->setDown(false);
    setCursor(Qt::ArrowCursor);
    
    if (m_mouse->lastClick() == m_mouse->widgetPosition()) {
        QToolTip::hideText();
        
        const MapItem* item = underPoint(m_mouse->widgetPosition());
        if (item) {
            emit mapItemClicked(item);
            
            if (const AirportItem* i1 = qobject_cast<const AirportItem*>(item))
                emit airportDetailsRequested(i1->airport());
            else if (const FlightItem* i2 = qobject_cast<const FlightItem*>(item))
                emit flightDetailsRequested(i2->pilot());
            else if (const FirItem* i3 = qobject_cast<const FirItem*>(item))
                emit firDetailsRequested(i3->fir());
        }
    }
    
    event->accept();
    update();
}

void MapWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        setCursor(Qt::SizeAllCursor);
        
        WorldTransform t = m_renderer->transform();
        QPoint diff = event->pos() - m_mouse->widgetPosition();
        if (!diff.isNull())
            renderer()->scene()->cancelFlightTracking();

        QPointF center = t.mapF(m_renderer->center());
        center -= diff;
        
        LonLat llcenter = t.mapF(center).bound();
        m_renderer->setCenter(llcenter);
    }
    
    m_mouse->update(event->pos());
    event->accept();
    update();
}

void MapWidget::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* menu = makeMenu(event->pos());
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->exec(event->globalPos());
}

void MapWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Shift) {
        m_selectAll = true;
    } else {
        QWidget::keyPressEvent(event);
    }
}

void MapWidget::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Shift) {
        m_selectAll = false;
    } else {
        QWidget::keyPressEvent(event);
    }
}

const MapItem* MapWidget::underPoint(const QPoint& widgetPosition) const
{
    Q_ASSERT(m_renderer);
    
    LonLat pos = m_renderer->transform().map(widgetPosition);
    const MapItem* item = m_renderer->scene()->nearest(pos.bound());
    if (item) {
        QSize size = item->size();
//        int m = 48 * qApp->primaryScreen()->devicePixelRatio();
//        size = size.expandedTo(QSize(m, m));
        QPoint p = renderer()->transform().map(item->position());
        QRect r(p.x() - size.width() / 2, p.y() - size.height() / 2, size.width(), size.height());
        return r.contains(widgetPosition) ? item : nullptr;
    } else {
        return nullptr;
    }
}

QMenu* MapWidget::makeMenu(const QPoint &pos)
{
    QMenu* menu = new QMenu(this);
    const MapItem* item = underPoint(pos);
    
    if (const AirportItem* airportItem = qobject_cast<const AirportItem*>(item)) {
        menu->setTitle(airportItem->airport()->icao());
        menu->addAction(tr("Airport details"),
                        std::bind(&MapWidget::airportDetailsRequested, this, airportItem->airport()));
        //: %1 is the airport ICAO code
        menu->addAction(tr("%1 METAR").arg(airportItem->airport()->icao()),
                        std::bind(&MapWidget::metarRequested, this, airportItem->airport()->icao()));
        
        auto staff = airportItem->airport()->clients<Atc>();
        if (!staff.isEmpty()) {
            //: a.k.a. ATC
            menu->addSection(tr("Controllers"));
            for (const Atc* a: staff)
                menu->addAction(a->callsign(), std::bind(&MapWidget::atcDetailsRequested, this, a));
        }
    } else if (const FlightItem* flightItem = qobject_cast<const FlightItem*>(item)) {
        menu->setTitle(flightItem->pilot()->callsign());
        menu->addAction(tr("Flight details"),
                        std::bind(&MapWidget::flightDetailsRequested, this, flightItem->pilot()));

        QAction* trackAction = new QAction(tr("Track this flight"), nullptr);
        trackAction->setCheckable(true);

        bool isTracked = renderer()->scene()->trackedFlight() == flightItem->pilot();
        trackAction->setChecked(isTracked);

        connect(trackAction, &QAction::triggered, [isTracked, flightItem, this]() {
            this->renderer()->scene()->setTrackedFlight(isTracked ? nullptr : flightItem->pilot());
        });

        menu->addAction(trackAction);
    } else if (const FirItem* firItem = qobject_cast<const FirItem*>(item)) {
        menu->setTitle(firItem->fir()->icao());
        menu->addAction(tr("FIR details"),
                        std::bind(&MapWidget::firDetailsRequested, this, firItem->fir()));
        
        auto staff = firItem->fir()->clients<Atc>();
        if (!staff.isEmpty()) {
            //: a.k.a. ATC
            menu->addSection(tr("Controllers"));
            for (const Atc* a: staff)
                menu->addAction(a->callsign(), std::bind(&MapWidget::atcDetailsRequested, this, a));
        }
    } else if (item == nullptr) {
        menu->setTitle(tr("Nearby"));
        auto nearby = renderer()->scene()->nearest(10, LonLat(renderer()->transform().map(pos)).bound());

        /* Group fetched items */
        QList<const AirportItem*> airports;
        QList<const FlightItem*> flights;
        QList<const FirItem*> firs;

        for (auto item: nearby) {
            if (const AirportItem* i1 = qobject_cast<const AirportItem*>(item))
                airports.append(i1);
            else if (const FlightItem* i2 = qobject_cast<const FlightItem*>(item))
                flights.append(i2);
            else if (const FirItem* i3 = qobject_cast<const FirItem*>(item))
                firs.append(i3);
        }

        if (!airports.isEmpty()) {
            menu->addSection(tr("Airports"));
            for (const AirportItem* ai: airports) {
                menu->addAction(ai->airport()->representativeName(), std::bind(&MapWidget::airportDetailsRequested, this, ai->airport()));
            }
        }

        if (!flights.isEmpty()) {
            menu->addSection(tr("Flights"));
            for (const FlightItem* fi: flights) {
                menu->addAction(fi->pilot()->callsign(), std::bind(&MapWidget::flightDetailsRequested, this, fi->pilot()));
            }
        }

        if (!firs.isEmpty()) {
            menu->addSection(tr("FIRs"));
            for (const FirItem* ri: firs) {
                QString text = QStringLiteral("%1 %2").arg(ri->fir()->icao(), ri->fir()->name());
                menu->addAction(text, std::bind(&MapWidget::firDetailsRequested, this, ri->fir()));
            }
        }
    } else {
        Q_UNREACHABLE();
    }
    
    return menu;
}

}} /* namespace Vatsinator::Widgets */
