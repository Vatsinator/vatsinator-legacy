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
#include <chrono>

#include "storage/settingsmanager.h"
#include "ui/map/airportitem.h"
#include "ui/map/firitem.h"
#include "ui/map/flightitem.h"
#include "ui/map/iconkeeper.h"
#include "ui/map/mapconfig.h"
#include "ui/map/mapitem.h"
#include "ui/map/mapscene.h"
#include "ui/map/modelmatcher.h"
#include "ui/map/uiritem.h"
#include "ui/map/worldpolygon.h"
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "maprenderer.h"

#ifndef GL_MULTISAMPLE
# define GL_MULTISAMPLE 0x809D
#endif

MapRenderer::MapRenderer(QObject* _parent) :
    QObject(_parent),
    __functions(new QOpenGLFunctions(QOpenGLContext::currentContext())),
    __world(new WorldPolygon(this)),
    __iconKeeper(new IconKeeper(this)),
    __modelMatcher(new ModelMatcher(this)),
    __scene(new MapScene(this)) {
  
  Q_ASSERT(QOpenGLShaderProgram::hasOpenGLShaderPrograms());
  __createShaderPrograms();
  __restoreSettings();
  
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
  
  glClearColor(__scene->settings().colors.seas.redF(),
               __scene->settings().colors.seas.greenF(),
               __scene->settings().colors.seas.blueF(),
               1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  Q_ASSERT(glGetError() == 0);
  
  /* For a really strong debug */
//   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

MapRenderer::~MapRenderer() {
  __storeSettings();
  delete __functions;
}

LonLat
MapRenderer::mapToLonLat(const QPoint& _point) {
  static constexpr qreal xFactor = MapConfig::longitudeMax() / (MapConfig::baseWindowWidth() / 2);
  static constexpr qreal yFactor = MapConfig::latitudeMax() / (MapConfig::baseWindowHeight() / 2);
  
  return LonLat(
      static_cast<qreal>(_point.x() - (__viewport.width() / 2)) * xFactor / static_cast<qreal>(zoom()) + center().x(),
      -static_cast<qreal>(_point.y() - (__viewport.height() / 2)) * yFactor / static_cast<qreal>(zoom()) + center().y()
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
      static_cast<int>((_point.x() - center().x()) * zoom() / xFactor) + (__viewport.width() / 2),
      static_cast<int>((-_point.y() + center().y()) * zoom() / yFactor) + (__viewport.height() / 2)
    );
}

QPointF
MapRenderer::glFromLonLat(const LonLat& _point) {
  return QPointF(
      (_point.x() - center().x() + static_cast<qreal>(__xOffset)) /
          MapConfig::longitudeMax() * zoom(),
      (_point.y() - center().y()) / MapConfig::latitudeMax() * zoom()
    );
}

void
MapRenderer::drawLines(const MapItem* _item) {
  static constexpr GLfloat linesZ = static_cast<GLfloat>(MapConfig::MapLayers::Lines);
  
  QMatrix4x4 mvp = __projection * __worldTransform;
  mvp.translate(QVector3D(0.0f, 0.0f, linesZ));
  
  __identityProgram->bind();
  __texturedProgram->enableAttributeArray(vertexLocation());
  __identityProgram->setUniformValue(__identityMatrixLocation, mvp);
  
  for (float o: __offsets) {
    __identityProgram->setUniformValue(__identityOffsetLocation, o);
    _item->drawFocused(__identityProgram);
  }
  
  __identityProgram->release();
}

void
MapRenderer::setZoom(int _zoom) {
  __zoom = _zoom;
  __updateScreen();
  emit updated();
}

void
MapRenderer::setCenter(const LonLat& _center) {
  __center = _center;
  __updateScreen();
  emit updated();
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
          __scene->settings().misc.zoom_coefficient * 0.01
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
        __scene->settings().misc.zoom_coefficient * 0.01
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
  __updateScreen();
  emit updated();
}

void
MapRenderer::paint() {
//   auto start = std::chrono::high_resolution_clock::now();
  if (__screen.isNull())
    return;
  
  __updateOffsets();
  
  /* Prepare world transform matrix */
  __worldTransform.setToIdentity();
  __worldTransform.scale(1.0f / MapConfig::longitudeMax(), 1.0f / MapConfig::latitudeMax());
  __worldTransform.scale(zoom(), zoom());
  __worldTransform.translate(-center().x(), -center().y());
  
  glClearColor(__scene->settings().colors.seas.redF(),
               __scene->settings().colors.seas.greenF(),
               __scene->settings().colors.seas.blueF(),
               1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  for (float o: __offsets) {
    __xOffset = o;
    
    __drawWorld();
    __drawUirs();
    __drawFirs();
    __drawItems();
  }
  
  __xOffset = 0.0f;
//   auto end = std::chrono::high_resolution_clock::now();
//   qDebug() << "MapRenderer::paint()" << std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
  Q_ASSERT(glGetError() == 0);
}

void
MapRenderer::__drawWorld() {
  static constexpr GLfloat zValue = static_cast<GLfloat>(MapConfig::MapLayers::WorldMap);
  
  QMatrix4x4 mvp = __projection * __worldTransform;
  mvp.translate(QVector3D(0.0f, 0.0f, zValue));
  __identityProgram->bind();
  __identityProgram->setUniformValue(__identityOffsetLocation, __xOffset);
  __identityProgram->setUniformValue(__identityMatrixLocation, mvp);
  __identityProgram->setUniformValue(__identityColorLocation, __scene->settings().colors.lands);
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
  
  if (__scene->settings().view.unstaffed_firs) {
    mvp.translate(QVector3D(0.0f, 0.0f, unstaffedFirsZ));
    __identityProgram->setUniformValue(__identityMatrixLocation, mvp);
    __identityProgram->setUniformValue(__identityColorLocation, __scene->settings().colors.unstaffed_fir_borders);
    for (const FirItem* item: __scene->firItems()) {
      if (item->data()->isEmpty())
        item->drawBorders();
    }
  }
  
  if (__scene->settings().view.staffed_firs) {
    mvp.translate(QVector3D(0.0f, 0.0f, __scene->settings().view.unstaffed_firs ? staffedFirsZ - unstaffedFirsZ : staffedFirsZ));
    __identityProgram->setUniformValue(__identityMatrixLocation, mvp);
    __identityProgram->setUniformValue(__identityColorLocation, __scene->settings().colors.staffed_fir_borders);
    
    glLineWidth(3.0);
    for (const FirItem* item: __scene->firItems()) {
      if (item->data()->isStaffed())
        item->drawBorders();
    }
    glLineWidth(1.0);
    
    __identityProgram->setUniformValue(__identityColorLocation, __scene->settings().colors.staffed_fir_background);
    for (const FirItem* item: __scene->firItems()) {
      if (item->data()->isStaffed())
        item->drawBackground();
    }
  }
  
  __identityProgram->release();
}

void
MapRenderer::__drawUirs() {
  static constexpr GLfloat staffedUirsZ = static_cast<GLfloat>(MapConfig::MapLayers::StaffedUirs);
  
  if (__scene->settings().view.staffed_firs) {
    __identityProgram->bind();
    __identityProgram->setUniformValue(__identityOffsetLocation, __xOffset);
    QMatrix4x4 mvp = __projection * __worldTransform;
    mvp.translate(QVector3D(0.0f, 0.0f, staffedUirsZ));
    
    __identityProgram->setUniformValue(__identityMatrixLocation, mvp);
    __identityProgram->setUniformValue(__identityColorLocation, __scene->settings().colors.staffed_uir_borders);
    
    glLineWidth(3.0);
    for (const UirItem* item: __scene->uirItems()) {
      if (item->isVisible()) {
        for (const FirItem* f: item->firItems()) {
          if (f->data()->isEmpty())
            f->drawBorders();
        }
      }
    }
    
    glLineWidth(1.0);
    
    __identityProgram->setUniformValue(__identityColorLocation, __scene->settings().colors.staffed_uir_background);
    for (const UirItem* item: __scene->uirItems()) {
      if (item->isVisible()) {
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
MapRenderer::__drawItems() {
  __texturedProgram->bind();
  __texturedProgram->setUniformValue(__texturedMatrixLocation, __projection);
  __texturedProgram->enableAttributeArray(texcoordLocation());
  __texturedProgram->enableAttributeArray(vertexLocation());
  
  scene()->forEachItem(__screen, [this](const MapItem* item) {
    QPointF p = glFromLonLat(item->position());
    __texturedProgram->setUniformValue(__texturedPositionLocation, p.x(), p.y());
    item->drawItem(__texturedProgram);
    if (item->isLabelVisible())
      item->drawLabel(__texturedProgram);
  });
  
  __texturedProgram->disableAttributeArray(texcoordLocation());
  __texturedProgram->disableAttributeArray(vertexLocation());
  __texturedProgram->release();
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
  bool result;
  /* Create identity shader */
  __identityProgram = new QOpenGLShaderProgram(this);
  QOpenGLShader* vertex = new QOpenGLShader(QOpenGLShader::Vertex, __identityProgram);
  result = vertex->compileSourceFile(":/shaders/identity.vert");
  Q_ASSERT(result);
  QOpenGLShader* fragment = new QOpenGLShader(QOpenGLShader::Fragment, __identityProgram);
  result = fragment->compileSourceFile(":/shaders/identity.frag");
  Q_ASSERT(result);
  __identityProgram->addShader(vertex);
  __identityProgram->addShader(fragment);
  
  __identityProgram->bindAttributeLocation("vertex", vertexLocation());
  
  result = __identityProgram->link();
  Q_ASSERT(result);
  __identityProgram->bind();
  __identityMatrixLocation = __identityProgram->uniformLocation("matrix");
  Q_ASSERT(__identityMatrixLocation >= 0);
  __identityColorLocation = __identityProgram->uniformLocation("color");
  Q_ASSERT(__identityColorLocation >= 0);
  __identityOffsetLocation = __identityProgram->uniformLocation("offset");
  Q_ASSERT(__identityOffsetLocation >= 0);
  __identityProgram->release();
  
  /* Create textured shader */
  __texturedProgram = new QOpenGLShaderProgram(this);
  vertex = new QOpenGLShader(QOpenGLShader::Vertex, __texturedProgram);
  result = vertex->compileSourceFile(":/shaders/textured.vert");
  Q_ASSERT(result);
  fragment = new QOpenGLShader(QOpenGLShader::Fragment, __texturedProgram);
  result = fragment->compileSourceFile(":/shaders/textured.frag");
  Q_ASSERT(result);
  __texturedProgram->addShader(vertex);
  __texturedProgram->addShader(fragment);
  
  __texturedProgram->bindAttributeLocation("vertex", vertexLocation());
  __texturedProgram->bindAttributeLocation("texcoord", texcoordLocation());
  
  result = __texturedProgram->link();
  Q_ASSERT(result);
  
  __texturedProgram->bind();
  __texturedMatrixLocation = __texturedProgram->uniformLocation("matrix");
  Q_ASSERT(__texturedMatrixLocation >= 0);
  __texturedPositionLocation = __texturedProgram->uniformLocation("position");
  Q_ASSERT(__texturedPositionLocation >= 0);
  __texturedRotationLocation = __texturedProgram->uniformLocation("rotation");
  Q_ASSERT(__texturedRotationLocation >= 0);
  __texturedZLocation = __texturedProgram->uniformLocation("z");
  Q_ASSERT(__texturedZLocation >= 0);
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

void
MapRenderer::__updateScreen() {
  __screen.setBottomLeft(mapToLonLat(QPoint(-10, __viewport.height() + 10)));
  __screen.setTopRight(mapToLonLat(QPoint(__viewport.width() + 10, -10)));
}