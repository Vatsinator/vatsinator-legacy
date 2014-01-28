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

#include <QtGlobal>
#include <QtGui>
#include <QtOpenGL>

#include "glutils/framebufferobject.h"
#include "glutils/glextensions.h"

#include "storage/settingsmanager.h"

#include "ui/map/mapconfig.h"
#include "ui/map/worldpolygon.h"
#include "ui/windows/vatsinatorwindow.h"

#include "vatsimdata/vatsimdatahandler.h"

#include "mapwidget.h"
#include "defines.h"

MapWidget::MapWidget(QWidget* _parent) :
    QGLWidget(MapConfig::glFormat(), _parent),
    __fbo(nullptr),
    __center(0.0, 0.0),
    __actualZoom(0),
    __zoom(1.0f) {
  
  connect(VatsimDataHandler::getSingletonPtr(), SIGNAL(vatsimDataUpdated()),
          this,                                 SLOT(redraw()));
  connect(SettingsManager::getSingletonPtr(),   SIGNAL(settingsChanged()),
          this,                                 SLOT(__reloadSettings()));
  
  setAutoBufferSwap(true);
}

MapWidget::~MapWidget() {
  delete __world;
}

QPointF
MapWidget::mapToLonLat(const QPoint& _point) {
  static constexpr qreal xFactor = MapConfig::longitudeMax() / (MapConfig::baseWindowWidth() / 2);
  static constexpr qreal yFactor = MapConfig::latitudeMax() / (MapConfig::baseWindowHeight() / 2);
  
  return QPointF(
      static_cast<qreal>(_point.x() - (width() / 2)) * xFactor / static_cast<qreal>(__zoom) + __center.x(),
      static_cast<qreal>(_point.y() - (height() / 2)) * yFactor / static_cast<qreal>(__zoom) + __center.y()
    );
}

QPointF
MapWidget::scaleToLonLat(const QPoint& _point) {
  static constexpr qreal xFactor = MapConfig::longitudeMax() / (MapConfig::baseWindowWidth() / 2);
  static constexpr qreal yFactor = MapConfig::latitudeMax() / (MapConfig::baseWindowHeight() / 2);
  
  return QPointF(
      static_cast<qreal>(_point.x()) * xFactor / static_cast<qreal>(__zoom),
      static_cast<qreal>(_point.y()) * yFactor / static_cast<qreal>(__zoom)
    );
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
  
  __world = new WorldPolygon();
  
  glShadeModel(GL_SMOOTH);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  /* For a really strong debug */
//   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void
MapWidget::paintGL() {
  __fbo->bind();
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(-__rangeX, __rangeX,
          -__rangeY, __rangeY,
          -1.0,      static_cast<GLdouble>(MapConfig::MapLayers::Count));
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  qglColor(__settings.colors.seas);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glEnableClientState(GL_VERTEX_ARRAY);
  
  qglClearColor(__settings.colors.seas);
  
  __drawWorld();
  
  glDisableClientState(GL_VERTEX_ARRAY);
  
  __fbo->unbind();
  
  __renderTexture();
}

void
MapWidget::resizeGL(int _width, int _height) {
  glViewport(0, 0, _width, _height);
  
  __rangeX = static_cast<qreal>(_width) / MapConfig::baseWindowWidth();
  __rangeY = static_cast<qreal>(_height) / MapConfig::baseWindowHeight();
  
  __updateFbo(_width, _height);
  updateGL();
}

void
MapWidget::wheelEvent(QWheelEvent* _event) {
  __updateZoom(_event->delta() / 120);
  updateGL();
  
  _event->accept();
}

void
MapWidget::mousePressEvent(QMouseEvent* _event) {
  __mousePosition = _event->pos();
  QToolTip::hideText();
  
  if (_event->buttons() & Qt::LeftButton)
    __lastClickPosition = _event->pos();
  
  _event->accept();
}

void
MapWidget::mouseReleaseEvent(QMouseEvent* _event) {
  __mousePosition = _event->pos();
  setCursor(QCursor(Qt::ArrowCursor));
  
  _event->accept();
}

void
MapWidget::mouseMoveEvent(QMouseEvent* _event) {
  if (_event->buttons() & Qt::LeftButton) {
    setCursor(QCursor(Qt::SizeAllCursor));
    
    QPoint diff = _event->pos() - __mousePosition;
    __center -= scaleToLonLat(diff);
    
    __center.ry() = qBound(-90.0, __center.y(), 90.0);
    if (__center.x() < -180.0)
      __center.rx() += 360.0;
    if (__center.x() > 180.0)
      __center.rx() -= 360.0;
  }
  
  __mousePosition = _event->pos();
  updateGL();
  
  QPointF coords = mapToLonLat(__mousePosition);
  VatsinatorWindow::getSingleton().positionBoxUpdate(coords.x(), coords.y());
  
  _event->accept();
}

void
MapWidget::__renderTexture() {
  static const float Coords[] = {
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f
  };
  
  static const float TexCoords[] = {
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f
  };
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
  
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_FLOAT, 0, Coords);
  
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, TexCoords);
  
  glBindTexture(GL_TEXTURE_2D, __fbo->texture());
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindTexture(GL_TEXTURE_2D, 0);
  
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

void
MapWidget::__drawWorld() {
  glPushMatrix();
    glScalef(1.0f / MapConfig::longitudeMax(), 1.0f / MapConfig::latitudeMax(), 1.0f);
    glScalef(__zoom, __zoom, 1.0f);
    glTranslated(-__center.x(), __center.y(), 0.0);
    
    glTranslatef(0.0, 0.0, static_cast<GLfloat>(MapConfig::MapLayers::WorldMap));
    
    qglColor(__settings.colors.lands);
    __world->paint();
  glPopMatrix();
}

void MapWidget::__updateFbo(int _width, int _height) {
  if (__fbo)
    delete __fbo;
  
  __fbo = new FrameBufferObject(_width, _height);
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
  __zoom = MapConfig::zoomMinimum() + MapConfig::zoomNormalizeCoef() *
      qPow(MapConfig::zoomBase() + (__settings.misc.zoom_coefficient * 0.01),
           (__actualZoom));
}

void
MapWidget::__reloadSettings() {
  __settings.misc.zoom_coefficient = SM::get("misc.zoom_coefficient").toInt();
  
  __settings.colors.lands = SM::get("colors.lands").value<QColor>();
  __settings.colors.seas = SM::get("colors.seas").value<QColor>();
  __settings.colors.staffed_fir_borders = SM::get("colors.staffed_fir_borders").value<QColor>();
  __settings.colors.staffed_fir_background = SM::get("colors.staffed_fir_background").value<QColor>();
  __settings.colors.staffed_uir_borders = SM::get("colors.staffed_uir_borders").value<QColor>();
  __settings.colors.staffed_uir_background = SM::get("colors.staffed_uir_background").value<QColor>();
  __settings.colors.unstaffed_fir_borders = SM::get("colors.unstaffed_fir_borders").value<QColor>();
  __settings.colors.approach_circle = SM::get("colors.approach_circle").value<QColor>();
  
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
