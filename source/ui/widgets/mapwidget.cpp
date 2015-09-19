/*
 * mapwidget.cpp
 * Copyright (C) 2013-2015  Michał Garapich <michal@garapich.pl>
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

#include <QtGlobal>
#include <QtWidgets>

#include "db/airportdatabase.h"
#include "events/mouselonlatevent.h"
#include "models/atctablemodel.h"
#include "storage/settingsmanager.h"
#include "ui/actions/actionmenuseparator.h"
#include "ui/actions/airportdetailsaction.h"
#include "ui/actions/clientdetailsaction.h"
#include "ui/actions/firdetailsaction.h"
#include "ui/actions/metaraction.h"
#include "ui/actions/trackaction.h"
#include "ui/map/airportitem.h"
#include "ui/map/firitem.h"
#include "ui/map/flightitem.h"
#include "ui/map/mapconfig.h"
#include "ui/map/mapitem.h"
#include "ui/map/maprenderer.h"
#include "ui/map/mapscene.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/widgetsuserinterface.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"
#include "config.h"

#include "plugins/tiled-map/tiledmapdrawer.h"

#include "mapwidget.h"

MapWidget::MapWidget(QWidget* parent) :
    QWidget(parent),
    __renderer(new MapRenderer())
{
    setAttribute(Qt::WA_AcceptTouchEvents);
    setAttribute(Qt::WA_TouchPadAcceptSingleTouchEvents);
    grabGesture (Qt::PinchGesture);
    
    connect(vApp()->vatsimDataHandler(), &VatsimDataHandler::vatsimStatusUpdated, this, static_cast<void (MapWidget::*)()>(&MapWidget::update));
    connect(vApp()->settingsManager(), &SettingsManager::settingsChanged, this, static_cast<void (MapWidget::*)()>(&MapWidget::update));
    
    __renderer->setMapDrawer(new TiledMapDrawer);
    connect(__renderer, &MapRenderer::updated, this, static_cast<void (MapWidget::*)()>(&MapWidget::update));
}

MapWidget::~MapWidget()
{
    delete __renderer;
}

bool
MapWidget::event(QEvent* event)
{
    switch (event->type()) {
        case QEvent::ToolTip: {
            QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
            const MapItem* item = __underPoint(helpEvent->pos());
            
            if (item) {
                QString text;
                if (const AirportItem* ai = qobject_cast<const AirportItem*>(item)) {
                    text = __toolTipForAirportItem(ai);
                } else if (const FlightItem* fi = qobject_cast<const FlightItem*>(item)) {
                    text = __toolTipForFlightItem(fi);
                } else if (const FirItem* fi = qobject_cast<const FirItem*>(item)) {
                    text = __toolTipForFirItem(fi);
                } else {
                    Q_UNREACHABLE();
                }
                QToolTip::showText(helpEvent->globalPos(), text);
            } else {
                QToolTip::hideText();
                event->ignore();
            }
            
            return true;
        }
        
        case QEvent::Gesture:
            return gestureEvent(static_cast<QGestureEvent*>(event));
            
        default:
            return QWidget::event(event);
    }
}

bool
MapWidget::gestureEvent(QGestureEvent* event)
{
    if (QGesture* pinch = event->gesture(Qt::PinchGesture)) {
        pinchTriggered(static_cast<QPinchGesture*> (pinch));
        event->accept(pinch);
    }
    
    return true;
}

void
MapWidget::pinchTriggered(QPinchGesture* gesture)
{
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
    
    if (changeFlags & QPinchGesture::ScaleFactorChanged) {
        qreal value = gesture->scaleFactor() - gesture->lastScaleFactor();
        __renderer->setZoom(qBound(static_cast<qreal> (MapConfig::zoomMinimum()),
                                   __renderer->zoom() + (__renderer->zoom() * value),
                                   static_cast<qreal> (MapConfig::zoomMaximum())));
    }
}

void
MapWidget::paintEvent(QPaintEvent* event)
{
    QSet<MapItem*> selected;
    
    if (cursor().shape() != Qt::SizeAllCursor) {
        const MapItem* item = __underMouse();    
        if (item) {
            setCursor (QCursor (Qt::PointingHandCursor));       
            selected.insert((MapItem*)item);
        } else {
            setCursor (QCursor (Qt::ArrowCursor));
        }
    }
    
    
    QPainter painter(this);
    __renderer->paint(&painter, selected);
    
#ifndef QT_NO_DEBUG
    QString mapInfo = QStringLiteral("Position: (%1, %2) zoom: %3, screen: (%4, %5), (%6, %7)").arg(
        QString::number(__renderer->center().x()),
        QString::number(__renderer->center().y()),
        QString::number(__renderer->zoom()),
        QString::number(__renderer->screen().topLeft().x()),
        QString::number(__renderer->screen().topLeft().y()),
        QString::number(__renderer->screen().bottomRight().x()),
        QString::number(__renderer->screen().bottomRight().y())
    );
    
    QPen pen(QColor(0, 0, 0));
    painter.setPen(pen);
    painter.drawText(rect(), Qt::AlignRight | Qt::AlignTop, mapInfo);
#endif
    
    /* TODO Update event->rect() instead of the whole widget */
    Q_UNUSED(event);
}

void
MapWidget::resizeEvent(QResizeEvent* event)
{
    __renderer->setViewport(event->size());
}

void
MapWidget::wheelEvent(QWheelEvent* event)
{
    if (event->phase() == Qt::ScrollUpdate)
        __updateZoom (event->delta() / 120);
        
    event->accept();
}

void
MapWidget::mousePressEvent(QMouseEvent* event)
{
    __mousePosition.update (event->pos());
    __mousePosition.setDown (true);
    QToolTip::hideText();
    
    if (event->buttons() & Qt::LeftButton)
        __lastClickPosition = event->pos();
    
    event->accept();
}

void
MapWidget::mouseReleaseEvent(QMouseEvent* event)
{
    __mousePosition.update (event->pos());
    __mousePosition.setDown (false);
    setCursor(QCursor(Qt::ArrowCursor));
    
    const MapItem* item = __underMouse();
    
    if (item) {
        if (__lastClickPosition == __mousePosition.screenPosition()) {
            QToolTip::hideText();
            if (const AirportItem* ai = qobject_cast<const AirportItem*>(item)) {
                wui()->showAirportDetails(ai->data());
            } else if (const FlightItem* fi = qobject_cast<const FlightItem*>(item)) {
                wui()->showClientDetails(fi->data());
            } else if (const FirItem* fi = qobject_cast<const FirItem*>(item)) {
                wui()->showFirDetails(fi->data());
            } else {
                Q_UNREACHABLE();
            }
        }
    } else {
        if (__renderer->scene()->trackedFlight()) {
            __renderer->scene()->moveTo(__renderer->scene()->trackedFlight()->position());
        } else if (__renderer->center().y() < -MapConfig::latitudeMax()) {
            __renderer->scene()->moveTo(LonLat(__renderer->center().longitude(), -MapConfig::latitudeMax()));
        } else if (__renderer->center().y() > MapConfig::latitudeMax()) {
            __renderer->scene()->moveTo(LonLat(__renderer->center().longitude(), MapConfig::latitudeMax()));
        }
    }
    
    event->accept();
}

void
MapWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        setCursor (QCursor (Qt::SizeAllCursor));
        QPoint diff = event->pos() - __mousePosition.screenPosition();
        QPoint center = __renderer->center() * __renderer->transform();
        center -= diff;
        
        LonLat llcenter = __renderer->mapToLonLat(center);
        llcenter.ry() = qBound(-90.0, llcenter.y(), 90.0);
        
        if (llcenter.x() < -180.0)
            llcenter.rx() += 360.0;
            
        if (llcenter.x() > 180.0)
            llcenter.rx() -= 360.0;
            
        __renderer->scene()->abortAnimation();
        __renderer->setCenter(llcenter);
    }
    
    __mousePosition.update(event->pos());
    update();
    event->accept();
}

void
MapWidget::keyPressEvent(QKeyEvent* event)
{
    switch (event->key()) {
        case Qt::Key_PageUp:
            __updateZoom (1);
            break;
            
        case Qt::Key_PageDown:
            __updateZoom (-1);
            break;
    }
}

void
MapWidget::contextMenuEvent(QContextMenuEvent* event)
{
    const MapItem* item = __underPoint(event->pos());
    
    QMenu* menu;
    if (!item) {
        menu = __menuForNoItem();
    } else if (const AirportItem* ai = qobject_cast<const AirportItem*>(item)) {
        menu = __menuForAirportItem(ai);
    } else if (const FlightItem* fi = qobject_cast<const FlightItem*>(item)) {
        menu = __menuForFlightItem(fi);
    } else if (const FirItem* fi = qobject_cast<const FirItem*>(item)) {
        menu = __menuForFirItem(fi);
    } else {
        Q_UNREACHABLE();
    }
    
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->exec(event->globalPos());
}

const MapItem*
MapWidget::__underMouse()
{
    const MapItem* closest = __renderer->scene()->nearest(__mousePosition.geoPosition());
    if (!closest)
        return nullptr;

    QPoint pos = closest->position() * __renderer->transform();
    
    if (__mousePosition.screenDistance(pos) > MapConfig::mouseOnObject())
        return nullptr;
    else
        return closest;
    
    return nullptr;
}

const MapItem*
MapWidget::__underPoint(const QPoint& point)
{
    const MapItem* closest = __renderer->scene()->nearest(__renderer->mapToLonLat (point).bound());
    Q_ASSERT(closest);
    QPoint pos = closest->position() * __renderer->transform();
    
    if (__mousePosition.screenDistance(pos) > MapConfig::mouseOnObject())
        return nullptr;
    else
        return closest;
    
    return nullptr;
}

void
MapWidget::__updateZoom(int steps)
{
    //   __renderer->updateZoom(_steps);
    __renderer->scene()->zoomTo(__renderer->zoomStep(static_cast<qreal>(steps)));
}

QMenu*
MapWidget::__menuForNoItem()
{
    /* Group fetched items */
    QList<const FlightItem*> flights;
    QList<const AirportItem*> airports;
    
    __renderer->scene()->nearTo(__mousePosition.geoPosition(), 10, [&](const MapItem* item) {
        if (const FlightItem* f = qobject_cast<const FlightItem*>(item)) {
            if (!f->data()->isPrefiledOnly())
                flights << f;
        } else if (const AirportItem* a = qobject_cast<const AirportItem*>(item)) {
            airports << a;
        }
    });
    
    QMenu* menu = new QMenu(tr("Nearby"), this);
    if (!flights.isEmpty()) {
        menu->addSection(tr("Flights"));
        std::for_each(flights.begin(), flights.end(), [this, menu](const FlightItem* item) {
            ClientDetailsAction* a = new ClientDetailsAction(item->data(), item->data()->callsign(), this);
            connect(a, &ClientDetailsAction::triggered, wui(), &WidgetsUserInterface::showClientDetails);
            menu->addAction(a);
        });
    }
    
    if (!airports.isEmpty()) {
        menu->addSection(tr("Airports"));
        std::for_each(airports.begin(), airports.end(), [this, menu](const AirportItem* item) {
            AirportDetailsAction* a = new AirportDetailsAction(item->data(), item->data()->icao(), this);
            connect(a, &AirportDetailsAction::triggered, wui(), &WidgetsUserInterface::showAirportDetails);
            menu->addAction(a);
        });
    }
    
    return menu;
}

QMenu*
MapWidget::__menuForAirportItem(const AirportItem* item)
{
    QMenu* menu = new QMenu(item->data()->icao(), this);
    
    AirportDetailsAction* showAp = new AirportDetailsAction(item->data(), tr("Airport details"), this);
    connect(showAp, &AirportDetailsAction::triggered,wui(), &WidgetsUserInterface::showAirportDetails);
    menu->addAction(showAp);
    
    MetarAction* showMetar = new MetarAction(item->data()->icao(), tr("%1 metar").arg(item->data()->icao()), this);
    connect(showMetar, &MetarAction::triggered, wui(), &WidgetsUserInterface::showMetar);
    menu->addAction(showMetar);
    
    if (!item->data()->staff()->toList().isEmpty()) {
        menu->addSection(tr("Controllers"));
        for (const Controller* c: item->data()->staff()->toList()) {
            ClientDetailsAction* a = new ClientDetailsAction(c, c->callsign(), this);
            connect(a, &ClientDetailsAction::triggered, wui(), &WidgetsUserInterface::showClientDetails);
            menu->addAction(a);
        }
    }
    
    if (item->data()->countArrivals() > 0) {
        menu->addSection(tr("Arrivals"));
        for (const Pilot* p: item->data()->inbounds()->toList()) {
            if (p->phase() == Pilot::Arrived) {
                ClientDetailsAction* a = new ClientDetailsAction(p, p->callsign(), this);
                connect(a, &ClientDetailsAction::triggered, wui(), &WidgetsUserInterface::showClientDetails);
                menu->addAction(a);
            }
        }
    }
    
    if (item->data()->countDepartures(false) > 0) {
        menu->addSection(tr("Departures"));
        for (const Pilot* p: item->data()->outbounds()->toList()) {
            if (p->phase() == Pilot::Departing) {
                ClientDetailsAction* a = new ClientDetailsAction(p, p->callsign(), this);
                connect(a, &ClientDetailsAction::triggered, wui(), &WidgetsUserInterface::showClientDetails);
                menu->addAction(a);
            }
        }
    }
    
    return menu;
}

QString
MapWidget::__toolTipForAirportItem(const AirportItem* item)
{
    const Airport* airport = item->data();
    QString desc = QStringLiteral("%1 %2, %3").arg(airport->icao(), airport->name(), airport->city());
    
    QString staff, deparr;
    if (!airport->isEmpty()) {
        for (const Controller* c: airport->staff()->toList()) {
            staff.append("<br>");
            staff.append(QStringLiteral("%1 %2 %3").arg(c->callsign(), c->frequency(), c->realName()));
        }
        
        int deps = airport->countDepartures();
        if (deps > 0) {
            deparr.append("<br>");
            deparr.append(tr("Departures: %1").arg(QString::number(deps)));
        }
        
        int arrs = airport->countArrivals();
        if (arrs > 0) {
            deparr.append("<br>");
            deparr.append(tr("Arrivals: %1").arg(QString::number(arrs)));
        }
    }
    return QStringLiteral("<p style='white-space:nowrap'><center>") % desc % staff % deparr % QStringLiteral("</center></p>");
}

QMenu*
MapWidget::__menuForFlightItem(const FlightItem* item)
{
    QMenu* menu = new QMenu(item->data()->callsign(), this);
    
    ClientDetailsAction* cda = new ClientDetailsAction(item->data(), tr("Flight details"), this);
    connect(cda, &ClientDetailsAction::triggered, wui(), &WidgetsUserInterface::showClientDetails);
    menu->addAction(cda);
    
    TrackAction* ta = new TrackAction(item->data(), this);
    menu->addAction(ta);
    
    return menu;
}

QString
MapWidget::__toolTipForFlightItem(const FlightItem* item)
{
    const Pilot* flight = item->data();
    QString desc = QStringLiteral("%1 (%2)").arg(flight->realName(), flight->aircraft());
    QString from = flight->origin() ? QStringLiteral("%1 %2").arg(flight->origin()->icao(), flight->origin()->city()) : flight->origin()->icao();
    QString to = flight->destination() ? QStringLiteral("%1 %2").arg(flight->destination()->icao(), flight->destination()->city()) : flight->destination()->icao();
    QString gs = tr("Ground speed: %1 kts").arg(QString::number(flight->groundSpeed()));
    QString alt = tr("Altitude: %1 ft").arg(QString::number(flight->altitude()));
    
    return QStringLiteral("<p style='white-space:nowrap'><center>%1<br>%2<br>%3 > %4<br>%5<br>%6</center></p>")
        .arg(flight->callsign(), desc, from, to, gs, alt);
}

QMenu*
MapWidget::__menuForFirItem(const FirItem* item)
{
    QMenu* menu = new QMenu(item->data()->icao(), this);
    
    FirDetailsAction* fa = new FirDetailsAction(item->data(), tr("%1 details").arg(item->data()->icao()), this);
    connect(fa, &FirDetailsAction::triggered, wui(), &WidgetsUserInterface::showFirDetails);
    menu->addAction(fa);
    
    for (const Controller* c: item->data()->staff()->toList()) {
        ClientDetailsAction* a = new ClientDetailsAction(c, c->callsign(), this);
        connect(a, &ClientDetailsAction::triggered, wui(), &WidgetsUserInterface::showClientDetails);
        menu->addAction(a);
    }
    
    for (const Controller* c: item->data()->uirStaff()->toList()) {
        ClientDetailsAction* a = new ClientDetailsAction(c, c->callsign(), this);
        connect(a, &ClientDetailsAction::triggered, wui(), &WidgetsUserInterface::showClientDetails);
        menu->addAction(a);
    }
    
    return menu;
}

QString
MapWidget::__toolTipForFirItem(const FirItem* item)
{
    const Fir* fir = item->data();
    QString desc;
    if (!fir->name().isEmpty()) {
        desc.append(fir->name());
        if (!fir->country().isEmpty()) {
            desc.append(", ").append(fir->country());
        }
    }
    
    QString staff;
    for (const Controller* c: fir->staff()->toList()) {
        staff.append(QStringLiteral("<br>%1 %2 %3").arg(c->callsign(), c->frequency(), c->realName()));
    }
    
    for (const Controller* c: fir->uirStaff()->toList()) {
        staff.append(QStringLiteral("<br>%1 %2 %3").arg(c->callsign(), c->frequency(), c->realName()));
    }
    
    if (desc.isEmpty() && staff.isEmpty())
        return QString();
    else
        return QStringLiteral("<p style='white-space:nowrap'><center>%1%2</center></p>")
            .arg(desc, staff);
}

MapWidget::MousePosition::MousePosition() : __down (false) {}

void
MapWidget::MousePosition::update(const QPoint& pos)
{
    __screenPosition = pos;
    __geoPosition = wui()->mainWindow()->mapWidget()->renderer()->mapToLonLat (pos).bound();
    
    MouseLonLatEvent e (__geoPosition);
    qApp->notify (wui()->mainWindow(), &e);
}

qreal
MapWidget::MousePosition::screenDistance(const QPoint& point)
{
    return qSqrt (
               qPow (point.x() - __screenPosition.x(), 2) +
               qPow (point.y() - __screenPosition.y(), 2)
           );
}

qreal
MapWidget::MousePosition::geoDistance(const LonLat& point)
{
    return qSqrt (
               qPow (point.x() - __geoPosition.x(), 2) +
               qPow (point.y() - __geoPosition.y(), 2)
           );
}

void
MapWidget::MousePosition::setDown(bool down)
{
    __down = down;
}
