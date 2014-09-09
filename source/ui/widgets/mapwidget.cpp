/*
 * mapwidget.cpp
 * Copyright (C) 2013-2014  Michał Garapich <michal@garapich.pl>
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

#include <QtWidgets>
#include <QtOpenGL>

#include "db/airportdatabase.h"
#include "events/mouselonlatevent.h"
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
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "mapwidget.h"

MapWidget::MapWidget(QWidget* _parent) :
    QGLWidget(MapConfig::glFormat(), _parent) {
  
  setAttribute(Qt::WA_NoSystemBackground);
  setAttribute(Qt::WA_OpaquePaintEvent);
  
  connect(vApp()->vatsimDataHandler(),  SIGNAL(vatsimDataUpdated()),
          this,                         SLOT(update()));
  
  connect(this, SIGNAL(menuRequest(const MapItem*)), SLOT(__showMenu(const MapItem*)));
  connect(this, SIGNAL(windowRequest(const MapItem*)),  SLOT(__showWindow(const MapItem*)));
  
  setAutoBufferSwap(true);
}

MapWidget::~MapWidget() {
  delete __renderer;
}

bool
MapWidget::event(QEvent* _event) {
  switch (_event->type()) {
    case QEvent::ToolTip: {
      QHelpEvent* helpEvent = static_cast<QHelpEvent*>(_event);
      const MapItem* item = __underPoint(helpEvent->pos());
      if (item) {
        QToolTip::showText(helpEvent->globalPos(), item->tooltipText());
      } else {
        QToolTip::hideText();
        _event->ignore();
      }
      
      return true;
    }
    default:
      return QGLWidget::event(_event);
  }
}

void
MapWidget::initializeGL() {
  __renderer = new MapRenderer();
  connect(__renderer,   SIGNAL(updated()),
          this,         SLOT(update()), Qt::DirectConnection);
}

void
MapWidget::paintGL() {
  __renderer->paint();
  
  const MapItem* item = __underMouse();
  if (item) {
    if (cursor().shape() != Qt::SizeAllCursor)
      setCursor(QCursor(Qt::PointingHandCursor));
    
    __renderer->drawLines(item);
  } else {
    if (cursor().shape() != Qt::SizeAllCursor)
      setCursor(QCursor(Qt::ArrowCursor));
  }
}

void
MapWidget::resizeGL(int _width, int _height) {
  __renderer->setViewport(QSize(_width, _height)); 
}

void
MapWidget::wheelEvent(QWheelEvent* _event) {
  __updateZoom(_event->delta() / 120);
  _event->accept();
}

void
MapWidget::mousePressEvent(QMouseEvent* _event) {
  __mousePosition.update(_event->pos());
  __mousePosition.setDown(true);
  QToolTip::hideText();
  
  if (_event->buttons() & Qt::LeftButton) {
    __lastClickPosition = _event->pos();
  } else if (_event->buttons() & Qt::RightButton) {
    const MapItem* item = __underMouse();
    emit menuRequest(item);
  }
  
  _event->accept();
}

void
MapWidget::mouseReleaseEvent(QMouseEvent* _event) {
  __mousePosition.update(_event->pos());
  __mousePosition.setDown(false);
  setCursor(QCursor(Qt::ArrowCursor));
  
  const MapItem* item = __underMouse();
  if (item) {
    if (__lastClickPosition == __mousePosition.screenPosition()) {
      QToolTip::hideText();
      emit windowRequest(item);
    }
  }
  
  _event->accept();
}

void
MapWidget::mouseMoveEvent(QMouseEvent* _event) {
  if (_event->buttons() & Qt::LeftButton) {
    setCursor(QCursor(Qt::SizeAllCursor));
    
    QPoint diff = _event->pos() - __mousePosition.screenPosition();
    diff.rx() *= -1;
    LonLat center = __renderer->center();
    center += __renderer->scaleToLonLat(diff);
    
    center.ry() = qBound(-90.0, center.y(), 90.0);
    if (center.x() < -180.0)
      center.rx() += 360.0;
    if (center.x() > 180.0)
      center.rx() -= 360.0;
    
    __renderer->scene()->abortAnimation();
    __renderer->setCenter(center);
  }
  __mousePosition.update(_event->pos());
  update();
  _event->accept();
}

void
MapWidget::keyPressEvent(QKeyEvent* _event) {
  switch (_event->key()) {
    case Qt::Key_PageUp:
    __updateZoom(1);
    break;
    
  case Qt::Key_PageDown:
    __updateZoom(-1);
    break;
  }
}

const MapItem*
MapWidget::__underMouse() {
  const MapItem* closest = __renderer->scene()->nearest(__mousePosition.geoPosition());
  Q_ASSERT(closest);
   
  if (__mousePosition.screenDistance(__renderer->mapFromLonLat(closest->position())) > MapConfig::mouseOnObject())
    return nullptr;
  else
    return closest;
}

const MapItem*
MapWidget::__underPoint(const QPoint& _p) {
  const MapItem* closest = __renderer->scene()->nearest(__renderer->mapToLonLat(_p).bound());
  Q_ASSERT(closest);
   
  if (__mousePosition.screenDistance(__renderer->mapFromLonLat(closest->position())) > MapConfig::mouseOnObject())
    return nullptr;
  else
    return closest;
}

void
MapWidget::__updateZoom(int _steps) {
  __renderer->updateZoom(_steps);
}

QMenu*
MapWidget::__itemMenu(const AirportItem* _item) {
  QMenu* menu = new QMenu(_item->data()->data()->icao, this);
  
  AirportDetailsAction* showAp = new AirportDetailsAction(_item->data(), tr("Airport details"), this);
  connect(showAp,                       SIGNAL(triggered(const Airport*)),
          vApp()->userInterface(),      SLOT(showDetails(const Airport*)));
  menu->addAction(showAp);
  
  MetarAction* showMetar = new MetarAction(_item->data()->data()->icao, this);
  connect(showMetar,                                    SIGNAL(triggered(QString)),
          vApp()->userInterface(),                      SLOT(showMetar(QString)));
  menu->addAction(showMetar);
  
  if (!_item->data()->isEmpty()) {
    if (!_item->data()->staff()->staff().isEmpty()) {
      menu->addSeparator();
      menu->addAction(new ActionMenuSeparator(tr("Controllers"), this));
      
      for (const Controller* c: _item->data()->staff()->staff()) {
        ClientDetailsAction* cda = new ClientDetailsAction(c, c->callsign(), this);
        connect(cda,                            SIGNAL(triggered(const Client*)),
                vApp()->userInterface(),        SLOT(showDetails(const Client*)));
        menu->addAction(cda);
      }
    }
    
    if (_item->data()->countArrivals() > 0) {
      menu->addSeparator();
      menu->addAction(new ActionMenuSeparator(tr("Arrivals"), this));
      
      for (const Pilot* p: _item->data()->inbounds()->flights()) {
        if (p->phase() == Pilot::Arrived) {
          ClientDetailsAction* cda = new ClientDetailsAction(p, p->callsign(), this);
          connect(cda,                          SIGNAL(triggered(const Client*)),
                  vApp()->userInterface(),      SLOT(showDetails(const Client*)));
          menu->addAction(cda);
        }
      }
    }
    
    if (_item->data()->countDepartures(false) > 0) {
      menu->addSeparator();
      menu->addAction(new ActionMenuSeparator(tr("Departures"), this));
      
      for (const Pilot* p: _item->data()->outbounds()->flights()) {
        if (!p->isPrefiledOnly() && p->phase() == Pilot::Departing) {
          ClientDetailsAction* cda = new ClientDetailsAction(p, p->callsign(), this);
          connect(cda,                          SIGNAL(triggered(const Client*)),
                  vApp()->userInterface(),      SLOT(showDetails(const Client*)));
          menu->addAction(cda);
        }
      }
    }
  }
  
  return menu;
}

QMenu*
MapWidget::__itemMenu(const FirItem* _item) {
  QMenu* menu = new QMenu(_item->data()->icao(), this);
  
  FirDetailsAction* showFir = new FirDetailsAction(
      _item->data(),
      tr("%1 details").arg(_item->data()->icao()),
      this
    );
  connect(showFir,                      SIGNAL(triggered(const Fir*)),
          vApp()->userInterface(),      SLOT(showDetails(const Fir*)));
  menu->addAction(showFir);
  
  for (const Controller* c: _item->data()->staff()->staff()) {
    ClientDetailsAction* cda = new ClientDetailsAction(c, c->callsign(), this);
    connect(cda,                        SIGNAL(triggered(const Client*)),
            vApp()->userInterface(),    SLOT(showDetails(const Client*)));
    menu->addAction(cda);
  }
  
  for (const Controller* c: _item->data()->uirStaff()->staff()) {
    ClientDetailsAction* cda = new ClientDetailsAction(c, c->callsign(), this);
    connect(cda,                        SIGNAL(triggered(const Client*)),
            vApp()->userInterface(),    SLOT(showDetails(const Client*)));
    menu->addAction(cda);
  }
  
  return menu;
}

QMenu*
MapWidget::__itemMenu(const FlightItem* _item) {
   QMenu* menu = new QMenu(_item->data()->callsign(), this);
  
  ClientDetailsAction* showDetails = new ClientDetailsAction(_item->data(), tr("Flight details"), this);
  connect(showDetails,                  SIGNAL(triggered(const Client*)),
          vApp()->userInterface(),      SLOT(showDetails(const Client*)));
  menu->addAction(showDetails);
  
  TrackAction* trackFlight = new TrackAction(_item->data(), this);
  menu->addAction(trackFlight);
  
  menu->addSeparator();

  if (!_item->data()->route().origin.isEmpty()) {
    MetarAction* ma = new MetarAction(_item->data()->route().origin, this);
    connect(ma,                                         SIGNAL(triggered(QString)),
            vApp()->userInterface(),                    SLOT(showMetar(QString)));
    menu->addAction(ma);
  }
  
  if (!_item->data()->route().destination.isEmpty()) {
    MetarAction* ma = new MetarAction(_item->data()->route().destination, this);
    connect(ma,                                         SIGNAL(triggered(QString)),
            vApp()->userInterface(),                    SLOT(showMetar(QString)));
    menu->addAction(ma);
  }
  
  return menu;
}

QMenu*
MapWidget::__itemMenu() {
  // fetch 10 nearest items
  QList<const MapItem*> items = __renderer->scene()->nearest(__mousePosition.geoPosition(), 10);
  
  QMenu* menu = new QMenu(tr("Nearby"), this);
  
  // group fetched items
  QList<const FlightItem*> flights;
  QList<const AirportItem*> airports;
  
  for (const MapItem* item: items) {
    if (const FlightItem* f = dynamic_cast<const FlightItem*>(item)) {
      if (!f->data()->isPrefiledOnly())
        flights << f;
    } else if (const AirportItem* a = dynamic_cast<const AirportItem*>(item)) {
      airports << a;
    }
  }
  
  if (flights.size() > 0) {
    menu->addAction(new ActionMenuSeparator(tr("Flights"), this));
    
    for (const FlightItem* f: flights) {
      ClientDetailsAction* action = new ClientDetailsAction(f->data(), f->data()->callsign(), this);
      connect(action,                   SIGNAL(triggered(const Client*)),
              vApp()->userInterface(),  SLOT(showDetails(const Client*)));
      menu->addAction(action);
    }
  }
  
  if (flights.size() > 0 && airports.size() >0)
    menu->addSeparator();
  
  if (airports.size() > 0) {
    menu->addAction(new ActionMenuSeparator(tr("Airports"), this));
    
    for (const AirportItem* a: airports) {
      AirportDetailsAction* action = new AirportDetailsAction(a->data(), a->data()->icao(), this);
      connect(action,                   SIGNAL(triggered(const Airport*)),
              vApp()->userInterface(),  SLOT(showDetails(const Airport*)));
      menu->addAction(action);
    }
  }
  
  return menu;
}

void
MapWidget::__showMenu(const MapItem* _item) {
  QMenu* menu;
  
  if (!_item)
    menu = __itemMenu();
  else if (const AirportItem* i = dynamic_cast<const AirportItem*>(_item))
    menu = __itemMenu(i);
  else if (const FirItem* i = dynamic_cast<const FirItem*>(_item))
    menu = __itemMenu(i);
  else if (const FlightItem* i = dynamic_cast<const FlightItem*>(_item))
    menu = __itemMenu(i);
  else
    Q_UNREACHABLE();
  
  menu->exec(mapToGlobal(__mousePosition.screenPosition()));
  delete menu;
}

void
MapWidget::__showWindow(const MapItem* _item) {
  _item->showDetails();
}

MapWidget::MousePosition::MousePosition() : __down(false) {}

void
MapWidget::MousePosition::update(const QPoint& _pos) {
  __screenPosition = _pos;
  __geoPosition = wui()->mainWindow()->mapWidget()->renderer()->mapToLonLat(_pos).bound();
  
  MouseLonLatEvent e(__geoPosition);
  qApp->notify(wui()->mainWindow(), &e);
}

qreal
MapWidget::MousePosition::screenDistance(const QPoint& _point) {
  return qSqrt(
    qPow(_point.x() - __screenPosition.x(), 2) +
    qPow(_point.y() - __screenPosition.y(), 2)
  );
}

qreal
MapWidget::MousePosition::geoDistance(const LonLat& _point) {
  return qSqrt(
    qPow(_point.x() - __geoPosition.x(), 2) +
    qPow(_point.y() - __geoPosition.y(), 2)
  );
}

void
MapWidget::MousePosition::setDown(bool _down) {
  __down = _down;
}
