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

#include "events/mapevent.h"
#include "events/mouselonlatevent.h"
#include "glutils/glextensions.h"
#include "storage/settingsmanager.h"
#include "ui/actions/clientdetailsaction.h"
#include "ui/map/airportitem.h"
#include "ui/map/approachcircleitem.h"
#include "ui/map/firitem.h"
#include "ui/map/flightitem.h"
#include "ui/map/mapconfig.h"
#include "ui/map/mapscene.h"
#include "ui/map/uiritem.h"
#include "ui/map/worldpolygon.h"
#include "ui/windows/vatsinatorwindow.h"
#include "ui/userinterface.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "mapwidget.h"

#ifndef GL_MULTISAMPLE
# define GL_MULTISAMPLE 0x809D
#endif

MapWidget::MapWidget(QWidget* _parent) :
    QGLWidget(MapConfig::glFormat(), _parent),
    __xOffset(0.0),
    __actualZoom(0),
    __world(nullptr),
    __scene(nullptr) {
  
  connect(VatsimDataHandler::getSingletonPtr(), SIGNAL(vatsimDataUpdated()),
          this,                                 SLOT(redraw()));
  connect(SettingsManager::getSingletonPtr(),   SIGNAL(settingsChanged()),
          this,                                 SLOT(__reloadSettings()));
  
  connect(this, SIGNAL(menuRequest(const MapItem*)), SLOT(__showMenu(const MapItem*)));
  connect(this, SIGNAL(menuRequest()), SLOT(__showMenu()));
  connect(this, SIGNAL(windowRequest(const MapItem*)),  SLOT(__showWindow(const MapItem*)));
  
  setAutoBufferSwap(true);
  
  __restoreSettings();
}

MapWidget::~MapWidget() {
  __storeSettings();
  
  delete __scene;
  delete __world;
}

LonLat
MapWidget::mapToLonLat(const QPoint& _point) {
  static constexpr qreal xFactor = MapConfig::longitudeMax() / (MapConfig::baseWindowWidth() / 2);
  static constexpr qreal yFactor = MapConfig::latitudeMax() / (MapConfig::baseWindowHeight() / 2);
  
  return LonLat(
      static_cast<qreal>(_point.x() - (width() / 2)) * xFactor / static_cast<qreal>(__state.zoom()) + __state.center().x(),
      -(static_cast<qreal>(_point.y() - (height() / 2)) * yFactor / static_cast<qreal>(__state.zoom()) + __state.center().y())
    );
}

LonLat
MapWidget::scaleToLonLat(const QPoint& _point) {
  static constexpr qreal xFactor = MapConfig::longitudeMax() / (MapConfig::baseWindowWidth() / 2);
  static constexpr qreal yFactor = MapConfig::latitudeMax() / (MapConfig::baseWindowHeight() / 2);
  
  return LonLat(
      static_cast<qreal>(_point.x()) * xFactor / static_cast<qreal>(__state.zoom()),
      static_cast<qreal>(_point.y()) * yFactor / static_cast<qreal>(__state.zoom())
    );
}

QPoint
MapWidget::mapFromLonLat(const LonLat& _point) {
  static constexpr qreal xFactor = MapConfig::longitudeMax() / (MapConfig::baseWindowWidth() / 2);
  static constexpr qreal yFactor = MapConfig::latitudeMax() / (MapConfig::baseWindowHeight() / 2);
  
  return QPoint(
      static_cast<int>((_point.x() - __state.center().x() + static_cast<qreal>(__xOffset)) * __state.zoom() / xFactor) + (width() / 2),
      static_cast<int>((-_point.y() - __state.center().y()) * __state.zoom() / yFactor) + (height() / 2)
    );
}

QPointF
MapWidget::glFromLonLat(const LonLat& _point) {
  return QPointF(
      (_point.x() - __state.center().x() + static_cast<qreal>(__xOffset)) /
          MapConfig::longitudeMax() * __state.zoom(),
      (_point.y() + __state.center().y()) / MapConfig::latitudeMax() * __state.zoom()
    );
}

bool
MapWidget::onScreen(const QPointF& _point) {
  return _point.x() <= __rangeX && _point.y() <= __rangeY &&
    _point.x() >= -__rangeX && _point.y() >= -__rangeY;
}

bool
MapWidget::event(QEvent* _e) {
  switch (_e->type()) {
    case Event::Map:
      return stateChangeEvent(dynamic_cast<MapEvent*>(_e));
    default:
      return QGLWidget::event(_e);
  }
}

void
MapWidget::redraw() {
  QToolTip::hideText();
  
  if (cursor().shape() != Qt::SizeAllCursor)
    setCursor(QCursor(Qt::ArrowCursor));
  
  updateGL();
}

void
MapWidget::initializeGL() {
  initGLExtensionsPointers();
  emit glReady();
  
  __world = new WorldPolygon();
  __scene = new MapScene(this);
  
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_LINE_STIPPLE);
  
  glShadeModel(GL_SMOOTH);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.1f);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  /* For a really strong debug */
//   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void
MapWidget::paintGL() {
  static const GLfloat textureCoords[] = {
    0.0, 0.0,
    0.0, 1.0,
    1.0, 1.0,
    1.0, 0.0
  };
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(-__rangeX, __rangeX,
          -__rangeY, __rangeY,
          -static_cast<GLdouble>(MapConfig::MapLayers::Count), 1.0);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  qglColor(__settings.colors.seas);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, textureCoords);
  
  qglClearColor(__settings.colors.seas);
  
  __underMouse = nullptr;
  
  glBindTexture(GL_TEXTURE_2D, 0);
  
  for (GLfloat o: __offsets) {
    __xOffset = o;
    
    __drawWorld();
    __drawUirs();
    __drawFirs();
    __drawAirports();
    __drawPilots();
  }
  
  for (GLfloat o: __offsets) {
    __xOffset = o;
    __drawLines();
  }
  
  __xOffset = 0.0f;
  
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
  
  if (__underMouse) {
    setCursor(QCursor(Qt::PointingHandCursor));
  } else {
    if (cursor().shape() != Qt::SizeAllCursor)
      setCursor(QCursor(Qt::ArrowCursor));
  }
  
  __updateTooltip();
}

void
MapWidget::resizeGL(int _width, int _height) {
  glViewport(0, 0, _width, _height);
  
  __rangeX = static_cast<qreal>(_width) / MapConfig::baseWindowWidth();
  __rangeY = static_cast<qreal>(_height) / MapConfig::baseWindowHeight();
  
  __updateOffsets();
  
  updateGL();
}

void
MapWidget::wheelEvent(QWheelEvent* _event) {
  __updateZoom(_event->delta() / 120);
  __updateOffsets();
  updateGL();
  
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
    if (__underMouse) {
      emit menuRequest(__underMouse);
    } else {
      emit menuRequest();
    }
  }
  
  _event->accept();
}

void
MapWidget::mouseReleaseEvent(QMouseEvent* _event) {
  __mousePosition.update(_event->pos());
  __mousePosition.setDown(false);
  setCursor(QCursor(Qt::ArrowCursor));
  
  if (__underMouse) {
    if (__lastClickPosition == __mousePosition.screenPosition()) {
      QToolTip::hideText();
      emit windowRequest(__underMouse);
    }
    
    __underMouse = nullptr;
  }
  
  _event->accept();
}

void
MapWidget::mouseMoveEvent(QMouseEvent* _event) {
  if ((_event->buttons() & Qt::LeftButton) && !scene()->animation()) {
    setCursor(QCursor(Qt::SizeAllCursor));
    
    QPoint diff = _event->pos() - __mousePosition.screenPosition();
    LonLat& center = __state.center();
    center -= scaleToLonLat(diff);
    
    center.ry() = qBound(-90.0, center.y(), 90.0);
    if (center.x() < -180.0)
      center.rx() += 360.0;
    if (center.x() > 180.0)
      center.rx() -= 360.0;
    
    __updateOffsets();
  }
  __mousePosition.update(_event->pos());
  updateGL();
  _event->accept();
}

bool
MapWidget::stateChangeEvent(MapEvent* _event) {
  if (!__mousePosition.down()) {
    __state = _event->state();
    _event->accept();
    redraw();
    return true;
  } else {
    _event->accept();
    return false;
  }
}

void
MapWidget::__drawWorld() {
  static constexpr GLfloat zValue = static_cast<GLfloat>(MapConfig::MapLayers::WorldMap);
  
  glPushMatrix();
    glScalef(1.0f / MapConfig::longitudeMax(), 1.0f / MapConfig::latitudeMax(), 1.0f);
    glScalef(__state.zoom(), __state.zoom(), 1.0f);
    glTranslated(-__state.center().x(), __state.center().y(), 0.0);
    
    glTranslatef(__xOffset, 0.0, zValue);
    
    qglColor(__settings.colors.lands);
    __world->paint();
  glPopMatrix();
}

void
MapWidget::__drawFirs() {
  static constexpr GLfloat unstaffedFirsZ = static_cast<GLfloat>(MapConfig::MapLayers::UnstaffedFirs);
  static constexpr GLfloat staffedFirsZ = static_cast<GLfloat>(MapConfig::MapLayers::StaffedFirs);
  
  glPushMatrix();
    glScalef(1.0f / MapConfig::longitudeMax(), 1.0f / MapConfig::latitudeMax(), 1.0f);
    glScalef(__state.zoom(), __state.zoom(), 1.0f);
    glTranslated(-__state.center().x(), __state.center().y(), 0.0);
    glTranslatef(__xOffset, 0.0, unstaffedFirsZ);
    
    if (__settings.view.unstaffed_firs) {
      qglColor(__settings.colors.unstaffed_fir_borders);
      for (const FirItem* item: __scene->firItems()) {
        if (item->data()->isEmpty())
          item->drawBorders();
      }
    }
    
    if (__settings.view.staffed_firs) {
      glTranslatef(0.0, 0.0, staffedFirsZ - unstaffedFirsZ);
      qglColor(__settings.colors.staffed_fir_borders);
      glLineWidth(3.0);
      for (const FirItem* item: __scene->firItems()) {
        if (item->data()->isStaffed())
          item->drawBorders();
      }
      
      glLineWidth(1.0);
      
      qglColor(__settings.colors.staffed_fir_background);
      for (const FirItem* item: __scene->firItems()) {
        if (item->data()->isStaffed())
          item->drawBackground();
      }
    }
  glPopMatrix();
  
  // draw labels
  glColor4f(1.0, 1.0, 1.0, 1.0);
  for (const FirItem* item: __scene->firItems()) {
    if (item->needsDrawing()) {
      QPointF p = glFromLonLat(item->position());
      if (onScreen(p)) {
        glPushMatrix();
          glTranslated(p.x(), p.y(), staffedFirsZ + 1);
          item->drawLabel();
        glPopMatrix();
        
      __checkItem(item);
      }
    }
  }
}

void
MapWidget::__drawUirs() {
  static constexpr GLfloat staffedUirsZ = static_cast<GLfloat>(MapConfig::MapLayers::StaffedUirs);
  
  if (__settings.view.staffed_firs) {
    glPushMatrix();
      glScalef(1.0f / MapConfig::longitudeMax(), 1.0f / MapConfig::latitudeMax(), 1.0f);
      glScalef(__state.zoom(), __state.zoom(), 1.0f);
      glTranslated(-__state.center().x(), __state.center().y(), 0.0);
      glTranslatef(__xOffset, 0.0, staffedUirsZ);
      
      qglColor(__settings.colors.staffed_uir_borders);
      glLineWidth(3.0);
      for (const UirItem* item: __scene->uirItems()) {
        if (item->needsDrawing()) {
          for (const FirItem* f: item->firItems()) {
            if (f->data()->isEmpty())
              f->drawBorders();
          }
        }
      }
      
      glLineWidth(1.0);
      
      qglColor(__settings.colors.staffed_fir_background);
      for (const UirItem* item: __scene->uirItems()) {
        if (item->needsDrawing()) {
          for (const FirItem* f: item->firItems()) {
            if (f->data()->isEmpty())
              f->drawBackground();
          }
        }
      }
    glPopMatrix();
  }
}

void
MapWidget::__drawAirports() {
  static constexpr GLfloat emptyAirportsZ = static_cast<GLfloat>(MapConfig::MapLayers::EmptyAirports);
  static constexpr GLfloat activeAirportsZ = static_cast<GLfloat>(MapConfig::MapLayers::ActiveAirports);
  
  if (__settings.view.empty_airports) {
    for (const AirportItem* item: __scene->airportItems()) {
      if (item->data()->isEmpty() && item->needsDrawing()) {
        QPointF p = glFromLonLat(item->position());
        if (onScreen(p)) {
          glPushMatrix();
            glTranslated(p.x(), -p.y(), emptyAirportsZ);
            item->drawIcon();
          glPopMatrix();
          
          __checkItem(item);
        }
      }
    }
  }
  
  if (__settings.view.airports_layer) {
    for (const AirportItem* item: __scene->airportItems()) {
      if (item->data()->isStaffed() && item->needsDrawing()) {
        QPointF p = glFromLonLat(item->position());
        if (onScreen(p)) {
          glPushMatrix();
            glTranslated(p.x(), p.y(), activeAirportsZ);
            
            item->drawIcon();
            
            if (__settings.view.airport_labels)
              item->drawLabel();
            
            if (item->approachCircle()) {
              glPushMatrix();
                glScalef(__state.zoom(), __state.zoom(), 0);
                item->approachCircle()->drawCircle();
              glPopMatrix();
            }
          
          glPopMatrix();
          
          __checkItem(item);
        }
      }
    }
  }
}

void
MapWidget::__drawPilots() {
  static constexpr GLfloat pilotsZ = static_cast<GLfloat>(MapConfig::MapLayers::Pilots);
  
  if (__settings.view.pilots_layer) {
    for (const FlightItem* item: __scene->flightItems()) {
      if (item->needsDrawing()) {
        QPointF p = glFromLonLat(item->position());
        if (onScreen(p)) {
          glPushMatrix();
            glTranslated(p.x(), p.y(), pilotsZ);
            item->drawModel();
            
            __checkItem(item);
            if (__shouldDrawPilotLabel(item))
              item->drawLabel();
            
          glPopMatrix();
        }
      }
    }
  }
}

void
MapWidget::__drawLines() {
  static constexpr GLfloat linesZ = static_cast<GLfloat>(MapConfig::MapLayers::Lines);
  
  if (__underMouse) {
    glPushMatrix();
      glScalef(1.0f / MapConfig::longitudeMax(), 1.0f / MapConfig::latitudeMax(), 1.0f);
      glScalef(__state.zoom(), __state.zoom(), 1.0f);
      glTranslated(-__state.center().x(), __state.center().y(), 0.0);
      glTranslatef(__xOffset, 0.0, linesZ);
      
      if (const FlightItem* pilot = dynamic_cast<const FlightItem*>(__underMouse)) {
        pilot->drawLines(FlightItem::OriginToPilot | FlightItem::PilotToDestination);
      } else if (const AirportItem* ap = dynamic_cast<const AirportItem*>(__underMouse)) {
        ap->drawLines();
      }
      
    glPopMatrix();
  }
}

void
MapWidget::__storeSettings() {
  QSettings settings;
  
  settings.beginGroup("CameraSettings");
  
  settings.setValue("actualZoomCoefficient", __actualZoom);
  settings.setValue("mapState", QVariant::fromValue<MapState>(__state));
  
  settings.endGroup();
}

void
MapWidget::__restoreSettings() {
  QSettings settings;
  
  settings.beginGroup("CameraSettings");
  
  __actualZoom = settings.value("actualZoomCoefficient", 0).toInt();
  __state = settings.value("mapState", QVariant::fromValue<MapState>(MapState())).value<MapState>();
  
  settings.endGroup();
}

void
MapWidget::__updateOffsets() {
  __offsets.clear();
  __offsets.append(0.0f);
  
  if ((-1 - __state.center().x()) * __state.zoom() > -__rangeX)
    __offsets.prepend(-360.0f);
  
  if ((1 - __state.center().x()) * __state.zoom() < __rangeX)
    __offsets.append(360.0f);
}

void
MapWidget::__updateZoom(int _steps) {
  //count limiter for this function
  __actualZoomMaximum =
      qFloor(qLn((MapConfig::zoomMaximum() - MapConfig::zoomMinimum()) / MapConfig::zoomNormalizeCoef()) /
      qLn(MapConfig::zoomBase() + (__settings.misc.zoom_coefficient * 0.01)));
  
  //set the actual zoom level according to number of scroll wheel steps
  __actualZoom += _steps;
  
  //limiting range of zoom
  __actualZoom = qBound(0, __actualZoom, __actualZoomMaximum);
  
  // count value of closeup
  __state.setZoom(
      MapConfig::zoomMinimum() + MapConfig::zoomNormalizeCoef() *
      qPow(MapConfig::zoomBase() + (__settings.misc.zoom_coefficient * 0.01),
          (__actualZoom)
        )
      );
}

void
MapWidget::__updateTooltip() {
   if (!__underMouse) {
    QToolTip::hideText();
   } else {
     QToolTip::showText(mapToGlobal(__mousePosition.screenPosition()), __underMouse->tooltipText());
   }
}

void
MapWidget::__checkItem(const MapItem* _item) {
  if (!__underMouse &&
      __mousePosition.screenDistance(
        mapFromLonLat(
          _item->position()
        )
      ) < MapConfig::mouseOnObject()) {
    __underMouse = _item;
  }
}

bool
MapWidget::__shouldDrawPilotLabel(const MapItem* _pilot) {
  Q_ASSERT(dynamic_cast<const FlightItem*>(_pilot));
  
  if (__settings.view.pilot_labels.always)
    return true;
  
  if (__settings.view.pilot_labels.when_hovered && __underMouse == _pilot)
    return true;
  
  return false;
}

void
MapWidget::__reloadSettings() {
  __settings.misc.zoom_coefficient = SM::get("map.zoom_coefficient").toInt();
  
  __settings.colors.lands = SM::get("map.lands_color").value<QColor>();
  __settings.colors.seas = SM::get("map.seas_color").value<QColor>();
  __settings.colors.staffed_fir_borders = SM::get("map.staffed_fir_borders_color").value<QColor>();
  __settings.colors.staffed_fir_background = SM::get("map.staffed_fir_background_color").value<QColor>();
  __settings.colors.staffed_uir_borders = SM::get("map.staffed_uir_borders_color").value<QColor>();
  __settings.colors.staffed_uir_background = SM::get("map.staffed_uir_background_color").value<QColor>();
  __settings.colors.unstaffed_fir_borders = SM::get("map.unstaffed_fir_borders_color").value<QColor>();
  __settings.colors.approach_circle = SM::get("map.approach_circle_color").value<QColor>();
  
  __settings.view.airports_layer = SM::get("view.airports_layer").toBool();
  __settings.view.airport_labels = SM::get("view.airport_labels").toBool();
  __settings.view.pilots_layer = SM::get("view.pilots_layer").toBool();
  __settings.view.staffed_firs = SM::get("view.staffed_firs").toBool();
  __settings.view.unstaffed_firs = SM::get("view.unstaffed_firs").toBool();
  __settings.view.empty_airports = SM::get("view.empty_airports").toBool();
  __settings.view.pilot_labels.always = SM::get("view.pilot_labels.always").toBool();
  __settings.view.pilot_labels.airport_related = SM::get("view.pilot_labels.airport_related").toBool();
  __settings.view.pilot_labels.when_hovered = SM::get("view.pilot_labels.when_hovered").toBool();
  
  redraw();
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
  QList<const MapItem*> items = scene()->items(rect);
  
  QMenu* menu = new QMenu(tr("Flights nearby"), this);
  
  for (const MapItem* item: items) {
    if (const FlightItem* f = dynamic_cast<const FlightItem*>(item)) {
      if (f->data()->isPrefiledOnly())
        continue;
      
      ClientDetailsAction* action = new ClientDetailsAction(f->data(), f->data()->callsign(), this);
      connect(action,                   SIGNAL(triggered(const Client*)),
              vApp()->userInterface(),  SLOT(showDetailsWindow(const Client*)));
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
  __geoPosition = MapWidget::getSingleton().mapToLonLat(_pos);
  
  MouseLonLatEvent e(__geoPosition);
  qApp->notify(vApp()->userInterface()->mainWindow(), &e);
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
