/*
 * maprenderer.cpp
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

#include <QtGui>

#include "storage/settingsmanager.h"
#include "ui/map/airportitem.h"
#include "ui/map/firitem.h"
#include "ui/map/flightitem.h"
#include "ui/map/iconkeeper.h"
#include "ui/map/mapconfig.h"
#include "ui/map/mapitem.h"
#include "ui/map/mapscene.h"
#include "ui/map/uiritem.h"
#include "ui/map/worldpolygon.h"
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/fir.h"
#include "vatsinatorapplication.h"

#include "maprenderer.h"

#ifndef GL_MULTISAMPLE
# define GL_MULTISAMPLE 0x809D
#endif

MapRenderer::MapRenderer(QObject* _parent) :
    QObject(_parent),
    __world(new WorldPolygon()),
    __iconKeeper(new IconKeeper(this)),
    __scene(new MapScene(this)) {
  
  __createShaderPrograms();
  __restoreSettings();
  
  connect(vApp()->settingsManager(),            SIGNAL(settingsChanged()),
          this,                                 SLOT(__reloadSettings()));
  __reloadSettings();
  
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
  
  glClearColor(__settings.colors.seas.redF(), __settings.colors.seas.greenF(), __settings.colors.seas.blueF(), 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  Q_ASSERT(glGetError() == 0);
  
  /* For a really strong debug */
//   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

MapRenderer::~MapRenderer() {
  __storeSettings();
}

LonLat
MapRenderer::mapToLonLat(const QPoint& _point) {
  static constexpr qreal xFactor = MapConfig::longitudeMax() / (MapConfig::baseWindowWidth() / 2);
  static constexpr qreal yFactor = MapConfig::latitudeMax() / (MapConfig::baseWindowHeight() / 2);
  
  return LonLat(
      static_cast<qreal>(_point.x() - (__viewport.width() / 2)) * xFactor / static_cast<qreal>(zoom()) + center().x(),
      -(static_cast<qreal>(_point.y() - (__viewport.height() / 2)) * yFactor / static_cast<qreal>(zoom()) + center().y())
    );
}

LonLat
MapRenderer::scaleToLonLat(const QPoint& _point) {
  static constexpr qreal xFactor = MapConfig::longitudeMax() / (MapConfig::baseWindowWidth() / 2);
  static constexpr qreal yFactor = MapConfig::latitudeMax() / (MapConfig::baseWindowHeight() / 2);
  
  return LonLat(
      static_cast<qreal>(_point.x()) * xFactor / static_cast<qreal>(zoom()),
      static_cast<qreal>(_point.y()) * yFactor / static_cast<qreal>(zoom())
    );
}

QPoint
MapRenderer::mapFromLonLat(const LonLat& _point) {
  static constexpr qreal xFactor = MapConfig::longitudeMax() / (MapConfig::baseWindowWidth() / 2);
  static constexpr qreal yFactor = MapConfig::latitudeMax() / (MapConfig::baseWindowHeight() / 2);
  
  return QPoint(
      static_cast<int>((_point.x() - center().x() + static_cast<qreal>(__xOffset)) * zoom() / xFactor) + (__viewport.width() / 2),
      static_cast<int>((-_point.y() - center().y()) * zoom() / yFactor) + (__viewport.height() / 2)
    );
}

QPointF
MapRenderer::glFromLonLat(const LonLat& _point) {
  return QPointF(
      (_point.x() - center().x() + static_cast<qreal>(__xOffset)) /
          MapConfig::longitudeMax() * zoom(),
      (_point.y() + center().y()) / MapConfig::latitudeMax() * zoom()
    );
}

bool
MapRenderer::onScreen(const QPointF& _point) {
  return _point.x() <= __rangeX && _point.y() <= __rangeY &&
    _point.x() >= -__rangeX && _point.y() >= -__rangeY;
}

void
MapRenderer::setZoom(int _zoom) {
  __zoom = _zoom;
}

void
MapRenderer::setCenter(const LonLat& _center) {
  __center = _center;
}

void
MapRenderer::updateZoom(int _steps) {
  //count limiter for this function
  __actualZoomMaximum =
    qFloor(
      qLn(
        (
          MapConfig::zoomMaximum() - MapConfig::zoomMinimum()
        ) /
          MapConfig::zoomNormalizeCoef()
      ) / qLn(
        MapConfig::zoomBase() +
        (
          __settings.misc.zoom_coefficient * 0.01
        )
      )
    );
  
  //set the actual zoom level according to number of scroll wheel steps
  __actualZoom += _steps;
  
  //limiting range of zoom
  __actualZoom = qBound(0, __actualZoom, __actualZoomMaximum);
  
  // count value of closeup
  setZoom(
    MapConfig::zoomMinimum() + MapConfig::zoomNormalizeCoef() *
    qPow(
      MapConfig::zoomBase() +
      (
        __settings.misc.zoom_coefficient * 0.01
      ),
      __actualZoom
    )
  );
}

void
MapRenderer::setViewport(const QSize& _size) {
  __viewport = _size;
  
  glViewport(0, 0, __viewport.width(), __viewport.height());
  
  __rangeX = static_cast<float>(__viewport.width()) / MapConfig::baseWindowWidth();
  __rangeY = static_cast<float>(__viewport.height()) / MapConfig::baseWindowHeight();
  
  __projection.setToIdentity();
  __projection.ortho(-__rangeX, __rangeX, -__rangeY, __rangeY, -static_cast<float>(MapConfig::MapLayers::Count), 1.0);
  
  __updateOffsets();
}

void
MapRenderer::paint() {
  /* Prepare world transform matrix */
  __worldTransform.setToIdentity();
  __worldTransform.scale(1.0f / MapConfig::longitudeMax(), 1.0f / MapConfig::latitudeMax());
  __worldTransform.scale(zoom(), zoom());
  __worldTransform.translate(-center().x(), center().y());
  
  glClearColor(__settings.colors.seas.redF(), __settings.colors.seas.greenF(), __settings.colors.seas.blueF(), 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  for (float o: __offsets) {
    __xOffset = o;
    
    __drawWorld();
    __drawUirs();
    __drawFirs();
    __drawAirports();
    __drawPilots();
  }
  
  for (float o: __offsets) {
    __xOffset = o;
    __drawLines();
  }
  
  __xOffset = 0.0f;
  
}

void
MapRenderer::__drawWorld() {
  static constexpr GLfloat zValue = static_cast<GLfloat>(MapConfig::MapLayers::WorldMap);
  
  QMatrix4x4 mvp = __projection * __worldTransform;
  mvp.translate(QVector3D(0.0f, 0.0f, zValue));
  __identityProgram->bind();
  __identityProgram->setUniformValue(__identityOffsetLocation, __xOffset);
  __identityProgram->setUniformValue(__identityMatrixLocation, mvp);
  __identityProgram->setUniformValue(__identityColorLocation, __settings.colors.lands);
  __world->paint();
  __identityProgram->release();
}

void
MapRenderer::__drawFirs() {
  static constexpr GLfloat unstaffedFirsZ = static_cast<GLfloat>(MapConfig::MapLayers::UnstaffedFirs);
  static constexpr GLfloat staffedFirsZ = static_cast<GLfloat>(MapConfig::MapLayers::StaffedFirs);
  
  QMatrix4x4 mvp = __projection * __worldTransform;
  __identityProgram->bind();
  __identityProgram->setUniformValue(__identityOffsetLocation, __xOffset);
    
  if (__settings.view.unstaffed_firs) {
    mvp.translate(QVector3D(0.0f, 0.0f, unstaffedFirsZ));
    __identityProgram->setUniformValue(__identityMatrixLocation, mvp);
    __identityProgram->setUniformValue(__identityColorLocation, __settings.colors.unstaffed_fir_borders);
    for (const FirItem* item: __scene->firItems()) {
      if (item->data()->isEmpty())
        item->drawBorders();
    }
  }
  
  if (__settings.view.staffed_firs) {
    mvp.translate(QVector3D(0.0f, 0.0f, __settings.view.unstaffed_firs ? staffedFirsZ - unstaffedFirsZ : staffedFirsZ));
    __identityProgram->setUniformValue(__identityMatrixLocation, mvp);
    __identityProgram->setUniformValue(__identityColorLocation, __settings.colors.staffed_fir_borders);
    
    glLineWidth(3.0);
    for (const FirItem* item: __scene->firItems()) {
      if (item->data()->isStaffed())
        item->drawBorders();
    }
    glLineWidth(1.0);
    
    __identityProgram->setUniformValue(__identityColorLocation, __settings.colors.staffed_fir_background);
    for (const FirItem* item: __scene->firItems()) {
      if (item->data()->isStaffed())
        item->drawBackground();
    }
  }
  
  __identityProgram->release();
  
  // draw labels
  mvp = __projection;
  mvp.translate(QVector3D(0.0f, 0.0f, staffedFirsZ + 1));
  __texturedProgram->bind();
  __texturedProgram->setUniformValue(__texturedMatrixLocation, mvp);
  __texturedProgram->enableAttributeArray(texcoordLocation());
  __texturedProgram->enableAttributeArray(vertexLocation());
  
  for (const FirItem* item: __scene->firItems()) {
    if (item->needsDrawing()) {
      QPointF p = glFromLonLat(item->position());
      if (onScreen(p)) {
        __texturedProgram->setUniformValue(__texturedPositionLocation, p.x(), p.y());
        item->drawLabel(__texturedProgram);
      }
    }
  }
  
  __texturedProgram->disableAttributeArray(texcoordLocation());
  __texturedProgram->disableAttributeArray(vertexLocation());
  __texturedProgram->release();
}

void
MapRenderer::__drawUirs() {
  static constexpr GLfloat staffedUirsZ = static_cast<GLfloat>(MapConfig::MapLayers::StaffedUirs);
  
  if (__settings.view.staffed_firs) {
    __identityProgram->bind();
    __identityProgram->setUniformValue(__identityOffsetLocation, __xOffset);
    QMatrix4x4 mvp = __projection * __worldTransform;
    mvp.translate(QVector3D(0.0f, 0.0f, staffedUirsZ));
    
    __identityProgram->setUniformValue(__identityMatrixLocation, mvp);
    __identityProgram->setUniformValue(__identityColorLocation, __settings.colors.staffed_uir_borders);
    
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
    
    __identityProgram->setUniformValue(__identityColorLocation, __settings.colors.staffed_uir_background);
    for (const UirItem* item: __scene->uirItems()) {
      if (item->needsDrawing()) {
        for (const FirItem* f: item->firItems()) {
          if (f->data()->isEmpty())
            f->drawBackground();
        }
      }
    }
    __identityProgram->release();
  }
}

void
MapRenderer::__drawAirports() {
  static constexpr GLfloat emptyAirportsZ = static_cast<GLfloat>(MapConfig::MapLayers::EmptyAirports);
  static constexpr GLfloat activeAirportsZ = static_cast<GLfloat>(MapConfig::MapLayers::ActiveAirports);
  
  QMatrix4x4 mvp = __projection;
  mvp.translate(0.0f, 0.0f, emptyAirportsZ);
  __texturedProgram->bind();
  __texturedProgram->setUniformValue(__texturedMatrixLocation, mvp);
  __texturedProgram->enableAttributeArray(texcoordLocation());
  __texturedProgram->enableAttributeArray(vertexLocation());
  
  if (__settings.view.empty_airports) {
    for (const AirportItem* item: __scene->airportItems()) {
      if (item->data()->isEmpty() && item->needsDrawing()) {
        QPointF p = glFromLonLat(item->position());
        if (onScreen(p)) {
          __texturedProgram->setUniformValue(__texturedPositionLocation, p.x(), p.y());
          item->drawIcon(__texturedProgram);
        }
      }
    }
  }
  
  mvp.translate(0.0f, 0.0f, activeAirportsZ - emptyAirportsZ);
  __texturedProgram->setUniformValue(__texturedMatrixLocation, mvp);
  if (__settings.view.airports_layer) {
    for (const AirportItem* item: __scene->airportItems()) {
      if (item->data()->isStaffed() && item->needsDrawing()) {
        QPointF p = glFromLonLat(item->position());
        if (onScreen(p)) {
          __texturedProgram->setUniformValue(__texturedPositionLocation, p.x(), p.y());
          item->drawIcon(__texturedProgram);
          
          if (__settings.view.airport_labels)
            item->drawLabel(__texturedProgram);
          /* TODO */
//           if (item->approachCircle()) {
//             glPushMatrix();
//               glScalef(__state.zoom(), __state.zoom(), 0);
//               item->approachCircle()->drawCircle();
//             glPopMatrix();
//           }
        }
      }
    }
  }
  
  __texturedProgram->disableAttributeArray(texcoordLocation());
  __texturedProgram->disableAttributeArray(vertexLocation());
  __texturedProgram->release();
}

void
MapRenderer::__drawPilots() {
  static constexpr GLfloat pilotsZ = static_cast<GLfloat>(MapConfig::MapLayers::Pilots);
  
  if (__settings.view.pilots_layer) {
    QMatrix4x4 mvp = __projection;
    mvp.translate(0.0f, 0.0f, pilotsZ);
    
    __texturedProgram->bind();
    __texturedProgram->setUniformValue(__texturedMatrixLocation, mvp);
    __texturedProgram->enableAttributeArray(texcoordLocation());
    __texturedProgram->enableAttributeArray(vertexLocation());
    
    for (const FlightItem* item: __scene->flightItems()) {
      if (item->needsDrawing()) {
        QPointF p = glFromLonLat(item->position());
        if (onScreen(p)) {
          __texturedProgram->setUniformValue(__texturedPositionLocation, p.x(), p.y());
          __texturedProgram->setUniformValue(__texturedRotationLocation, -qDegreesToRadians(static_cast<float>(item->data()->heading())));
          item->drawModel(__texturedProgram);
          
          if (__shouldDrawPilotLabel(item)) {
            __texturedProgram->setUniformValue(__texturedRotationLocation, 0.0f);
            item->drawLabel(__texturedProgram);
          }
        }
      }
    }
    
    __texturedProgram->setUniformValue(__texturedRotationLocation, 0.0f);
    __texturedProgram->release();
  }
}

void
MapRenderer::__drawLines() {
  static constexpr GLfloat linesZ = static_cast<GLfloat>(MapConfig::MapLayers::Lines);
  
//   if (__underMouse) {
//     QMatrix4x4 mvp = __projection * __worldTransform;
//     mvp.translate(QVector3D(0.0f, 0.0f, linesZ));
//     
//     __identityShader->bind();
//     __texturedShader->enableAttributeArray(vertexLocation());
//     __identityShader->setUniformValue(__identityOffsetLocation, __xOffset);
//     __identityShader->setUniformValue(__identityMatrixLocation, mvp);
//     
//     if (const FlightItem* pilot = dynamic_cast<const FlightItem*>(__underMouse)) {
//       pilot->drawLines(FlightItem::OriginToPilot | FlightItem::PilotToDestination, __identityShader);
//     } else if (const AirportItem* ap = dynamic_cast<const AirportItem*>(__underMouse)) {
//       ap->drawLines();
//     }
//     
//     __identityShader->release();
//   }
}

void
MapRenderer::__storeSettings() {
  QSettings settings;
  
  settings.beginGroup("CameraSettings");
  
  settings.setValue("actualZoomCoefficient", __actualZoom);
  settings.setValue("zoom", __zoom);
  settings.setValue("center", QVariant::fromValue<LonLat>(__center));
  
  settings.endGroup();
}

void
MapRenderer::__restoreSettings() {
  QSettings settings;
  
  settings.beginGroup("CameraSettings");
  
  __actualZoom = settings.value("actualZoomCoefficient", 0).toInt();
  __zoom = settings.value("zoom", 1).toInt();
  __center = settings.value("center", QVariant::fromValue<LonLat>(LonLat(0.0, 0.0))).value<LonLat>();
  
  settings.endGroup();
}

void
MapRenderer::__createShaderPrograms() {
  /* Create identity shader */
  __identityProgram = new QOpenGLShaderProgram(this);
  QOpenGLShader* vertex = new QOpenGLShader(QOpenGLShader::Vertex, __identityProgram);
  vertex->compileSourceFile(":/shaders/identity.vert");
  QOpenGLShader* fragment = new QOpenGLShader(QOpenGLShader::Fragment, __identityProgram);
  fragment->compileSourceFile(":/shaders/identity.frag");
  __identityProgram->addShader(vertex);
  __identityProgram->addShader(fragment);
  
  __identityProgram->bindAttributeLocation("vertex", vertexLocation());
  
  __identityProgram->link();
  __identityProgram->bind();
  __identityMatrixLocation = __identityProgram->uniformLocation("matrix");
  __identityColorLocation = __identityProgram->uniformLocation("color");
  __identityOffsetLocation = __identityProgram->uniformLocation("offset");
  __identityProgram->release();
  
  /* Create textured shader */
  __texturedProgram = new QOpenGLShaderProgram(this);
  vertex = new QOpenGLShader(QOpenGLShader::Vertex, __texturedProgram);
  vertex->compileSourceFile(":/shaders/textured.vert");
  fragment = new QOpenGLShader(QOpenGLShader::Fragment, __texturedProgram);
  fragment->compileSourceFile(":/shaders/textured.frag");
  __texturedProgram->addShader(vertex);
  __texturedProgram->addShader(fragment);
  
  __texturedProgram->bindAttributeLocation("vertex", vertexLocation());
  __texturedProgram->bindAttributeLocation("texcoord", texcoordLocation());
  
  __texturedProgram->link();
  __texturedProgram->bind();
  __texturedMatrixLocation = __texturedProgram->uniformLocation("matrix");
  __texturedPositionLocation = __texturedProgram->uniformLocation("position");
  __texturedRotationLocation = __texturedProgram->uniformLocation("rotation");
  __texturedProgram->setUniformValue("texture", 0);
  __texturedProgram->release();
}

void
MapRenderer::__updateOffsets() {
  __offsets.clear();
  __offsets.append(0.0f);
  
  if ((-1 - center().x()) * zoom() > -__rangeX)
    __offsets.prepend(-360.0f);
  
  if ((1 - center().x()) * zoom() < __rangeX)
    __offsets.append(360.0f);
}

bool
MapRenderer::__shouldDrawPilotLabel(const MapItem* _item) {
  Q_ASSERT(dynamic_cast<const FlightItem*>(_pilot));
  
  if (__settings.view.pilot_labels.always)
    return true;
  
//   if (__settings.view.pilot_labels.when_hovered)
//     return true;
  
  return false;
}

void
MapRenderer::__reloadSettings() {
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
}
