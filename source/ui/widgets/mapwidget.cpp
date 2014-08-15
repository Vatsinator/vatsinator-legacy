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

#include <QtGlobal>
#include <QtWidgets>
#include <QtOpenGL>

#include "events/mouselonlatevent.h"
#include "glutils/glextensions.h"
#include "storage/settingsmanager.h"
#include "ui/actions/clientdetailsaction.h"
#include "ui/map/flightitem.h"
#include "ui/map/mapconfig.h"
#include "ui/map/mapitem.h"
#include "ui/map/maprenderer.h"
#include "ui/map/mapscene.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/widgetsuserinterface.h"
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "mapwidget.h"

MapWidget::MapWidget(QWidget* _parent) :
    QGLWidget(MapConfig::glFormat(), _parent) {
  
  connect(vApp()->vatsimDataHandler(),          SIGNAL(vatsimDataUpdated()),
          this,                                 SLOT(redraw()));
  
  connect(this, SIGNAL(menuRequest(const MapItem*)), SLOT(__showMenu(const MapItem*)));
  connect(this, SIGNAL(menuRequest()), SLOT(__showMenu()));
  connect(this, SIGNAL(windowRequest(const MapItem*)),  SLOT(__showWindow(const MapItem*)));
  
  setAutoBufferSwap(true);
}

MapWidget::~MapWidget() {
  delete __renderer;
}

void
MapWidget::redraw() {
  QToolTip::hideText();
  
  if (cursor().shape() != Qt::SizeAllCursor)
    setCursor(QCursor(Qt::ArrowCursor));
  
  update();
}

void
MapWidget::initializeGL() {
  initGLExtensionsPointers();
  
  __renderer = new MapRenderer();
  connect(__renderer,           SIGNAL(updated()),
          this,                 SLOT(update()), Qt::DirectConnection);
  
  emit glReady();
}

void
MapWidget::paintGL() {
  __renderer->paint();
  
//   if (__renderer->underFocus()) {
//     setCursor(QCursor(Qt::PointingHandCursor));
//   } else {
//     if (cursor().shape() != Qt::SizeAllCursor)
//       setCursor(QCursor(Qt::ArrowCursor));
//   }
}

void
MapWidget::resizeGL(int _width, int _height) {
  __renderer->setViewport(QSize(_width, _height)); 
  redraw();
}

void
MapWidget::wheelEvent(QWheelEvent* _event) {
  __updateZoom(_event->delta() / 120);
  redraw();
  
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
    QToolTip::hideText();
//     if (__underMouse) {
//       emit menuRequest(__underMouse);
//     } else {
//       emit menuRequest();
//     }
  }
  
  _event->accept();
}

void
MapWidget::mouseReleaseEvent(QMouseEvent* _event) {
  __mousePosition.update(_event->pos());
  __mousePosition.setDown(false);
  setCursor(QCursor(Qt::ArrowCursor));
  /*
  if (__underMouse) {
    if (__lastClickPosition == __mousePosition.screenPosition()) {
      QToolTip::hideText();
      emit windowRequest(__underMouse);
    }
  }*/
  
  _event->accept();
}

void
MapWidget::mouseMoveEvent(QMouseEvent* _event) {
  if (_event->buttons() & Qt::LeftButton) {
    setCursor(QCursor(Qt::SizeAllCursor));
    
    QPoint diff = _event->pos() - __mousePosition.screenPosition();
    LonLat center = __renderer->center();
    center -= __renderer->scaleToLonLat(diff);
    
    center.ry() = qBound(-90.0, center.y(), 90.0);
    if (center.x() < -180.0)
      center.rx() += 360.0;
    if (center.x() > 180.0)
      center.rx() -= 360.0;
    
    __renderer->setCenter(center);
  }
  __mousePosition.update(_event->pos());
  update();
  _event->accept();
}

void
MapWidget::__updateZoom(int _steps) {
  __renderer->updateZoom(_steps);
}

void
MapWidget::__showMenu(const MapItem* _item) {  
  QMenu* menu = _item->menu(this);
  menu->exec(mapToGlobal(__mousePosition.screenPosition()));
  delete menu;
}

void
MapWidget::__showMenu() {
  QRectF rect(QPointF(), QSize(1.0f, 1.0f));
  rect.moveCenter(__mousePosition.geoPosition());
  QList<const MapItem*> items = __renderer->scene()->items(rect);
  
  QMenu* menu = new QMenu(tr("Flights nearby"), this);
  
  for (const MapItem* item: items) {
    if (const FlightItem* f = dynamic_cast<const FlightItem*>(item)) {
      if (f->data()->isPrefiledOnly())
        continue;
      
      ClientDetailsAction* action = new ClientDetailsAction(f->data(), f->data()->callsign(), this);
      connect(action,                   SIGNAL(triggered(const Client*)),
              vApp()->userInterface(),  SLOT(showDetails(const Client*)));
      menu->addAction(action);
    }
  }
  
  menu->exec(mapToGlobal(__mousePosition.screenPosition()));
  delete menu;
}

void
MapWidget::__showWindow(const MapItem* _item) {
  _item->showDetailsWindow();
}

MapWidget::MousePosition::MousePosition() : __down(false) {}

void
MapWidget::MousePosition::update(const QPoint& _pos) {
  __screenPosition = _pos;
  __geoPosition = wui()->mainWindow()->mapWidget()->renderer()->mapToLonLat(_pos);
  
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
