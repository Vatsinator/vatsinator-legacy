/*
 * firitem.cpp
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

#include <QtCore>
#include <QtOpenGL>

#include "db/firdatabase.h"
#include "glutils/texture.h"
#include "storage/settingsmanager.h"
#include "ui/map/mapconfig.h"
#include "ui/map/maprenderer.h"
#include "ui/map/mapscene.h"
#include "ui/userinterface.h"
#include "vatsimdata/fir.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsinatorapplication.h"

#include "firitem.h"

FirItem::FirItem(const Fir* _fir, QObject* _parent) :
    QObject(_parent),
    __scene(qobject_cast<MapScene*>(_parent)),
    __fir(_fir),
    __position(_fir->data()->header.textPosition.x, _fir->data()->header.textPosition.y),
    __borders(QOpenGLBuffer::VertexBuffer),
    __triangles(QOpenGLBuffer::IndexBuffer),
    __label(QOpenGLTexture::Target2D) {
  __initializeBuffers();
  
  connect(vApp()->settingsManager(),            SIGNAL(settingsChanged()),
          this,                                 SLOT(__resetLabel()));
  connect(__fir,                                SIGNAL(updated()),
          this,                                 SLOT(__invalidate()));
}

FirItem::~FirItem() {
  __label.destroy();
  __borders.destroy();
  __triangles.destroy();
}

void
FirItem::drawBorders() const {
  __vaoBorders.bind();
  glDrawArrays(GL_LINE_LOOP, 0, __bordersVertices);
  __vaoBorders.release();
}

void
FirItem::drawBackground() const {
  __vaoTriangles.bind();
  glDrawElements(GL_TRIANGLES, __trianglesVertices, GL_UNSIGNED_SHORT, 0);
  __vaoTriangles.release();
}

bool
FirItem::isVisible() const {
  if (position().isNull())
    return false;
  
  if (data()->isEmpty())
    return __scene->settings().view.unstaffed_firs;
  else
    return __scene->settings().view.staffed_firs;
}

bool
FirItem::isLabelVisible() const {
  if (data()->isEmpty())
    return __scene->settings().view.unstaffed_firs;
  else
    return __scene->settings().view.staffed_firs;
}

const LonLat &
FirItem::position() const {
  return __position;
}

void
FirItem::drawItem(QOpenGLShaderProgram* _shader) const {
  static constexpr float FirsZ = static_cast<float>(MapConfig::MapLayers::StaffedFirs + 1);
  
  static const GLfloat labelRect[] = {
    -0.08f, -0.05333333f,
    -0.08f,  0.05333333f,
     0.08f,  0.05333333f,
     0.08f,  0.05333333f,
     0.08f, -0.05333333f,
    -0.08f, -0.05333333f
  };
  
  static const GLfloat textureCoords[] = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f
  };
  
  if (!__label.isCreated())
    __initializeLabel();
  
  _shader->setAttributeArray(MapRenderer::texcoordLocation(), textureCoords, 2);
  _shader->setAttributeArray(MapRenderer::vertexLocation(), labelRect, 2);
  _shader->setUniformValue(__scene->renderer()->programZLocation(), FirsZ);
  
  __label.bind();
  glDrawArrays(GL_TRIANGLES, 0, 6);
//   __label.release();
}

void FirItem::drawLabel(QOpenGLShaderProgram*) const {
  
}

void FirItem::drawFocused(QOpenGLShaderProgram*) const {

}

QString
FirItem::tooltipText() const {
  QString desc;
  if (!data()->name().isEmpty()) {
    desc.append(data()->name());
    
    if (!data()->country().isEmpty()) {
      desc.append(", ");
      desc.append(data()->country());
    }
  }
  
  QString staff;
  for (const Controller* c: data()->staff()->staff()) {
    staff.append("<br>");
    staff.append(QString("%1 %2 %3").arg(c->callsign(), c->frequency(), c->realName()));
  }
  
  for (const Controller* c: data()->uirStaff()->staff()) {
    staff.append("<br>");
    staff.append(QString("%1 %2 %3").arg(c->callsign(), c->frequency(), c->realName()));
  }
  
  if (desc.isEmpty() && staff.isEmpty())
    return QString();
  else
    return QString("<p style='white-space:nowrap'><center>") % desc % staff % QString("</center></p>");
}

void
FirItem::showDetails() const {
  vApp()->userInterface()->showDetails(data());
}

void
FirItem::__initializeBuffers() {
  auto& borders = __fir->data()->borders;
  auto& triangles = __fir->data()->triangles;
  
  __borders.create();
  Q_ASSERT(__borders.isCreated());
  __borders.setUsagePattern(QOpenGLBuffer::StaticDraw);
  __borders.bind();
  __borders.allocate(borders.constData(), sizeof(Point) * borders.size());
  __borders.release();
  
  __triangles.create();
  Q_ASSERT(__triangles.isCreated());
  __triangles.setUsagePattern(QOpenGLBuffer::StaticDraw);
  __triangles.bind();
  __triangles.allocate(triangles.constData(), sizeof(unsigned int) * triangles.size());
  __triangles.release();
  
  __vaoBorders.create();
  Q_ASSERT(__vaoBorders.isCreated());
  __vaoBorders.bind();
  __borders.bind();
  __scene->renderer()->opengl()->glVertexAttribPointer(MapRenderer::vertexLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);
  __scene->renderer()->opengl()->glEnableVertexAttribArray(MapRenderer::vertexLocation());
  __vaoBorders.release();
  __borders.release();
  __triangles.release();
  
  __vaoTriangles.create();
  Q_ASSERT(__vaoTriangles.isCreated());
  __vaoTriangles.bind();
  __borders.bind();
  __triangles.bind();
  __scene->renderer()->opengl()->glVertexAttribPointer(MapRenderer::vertexLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);
  __scene->renderer()->opengl()->glEnableVertexAttribArray(MapRenderer::vertexLocation());
  __vaoTriangles.release();
  __borders.release();
  __triangles.release();
  
  __bordersVertices = borders.size();
  __trianglesVertices = triangles.size();
}

void
FirItem::__initializeLabel() const {
  static QRect labelRect(0, 4, 64, 24);
  
  if (__label.isCreated())
    __label.destroy();
  
  QString icao(__fir->icao());
  if (__fir->isOceanic())
    icao = icao.left(4) + " Oceanic";
  
  icao = icao.simplified();
  
  QImage temp(MapConfig::firLabelBackground());
  QPainter painter(&temp);
  painter.setRenderHint(QPainter::TextAntialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.setRenderHint(QPainter::HighQualityAntialiasing);
  
  painter.setFont(SM::get("map.fir_font").value<QFont>());
  
  QColor color;
  if (__fir->isStaffed()) {
    color = SM::get("map.staffed_fir_borders_color").value<QColor>();
  } else {
    if (__fir->uirStaff()->rowCount() > 0)
      color = SM::get("map.staffed_uir_borders_color").value<QColor>();
    else
      color = SM::get("map.unstaffed_fir_borders_color").value<QColor>();
  }
  
  painter.setPen(color);
  painter.drawText(labelRect, Qt::AlignCenter | Qt::TextWordWrap, icao);
  
  __label.setData(temp.mirrored(), QOpenGLTexture::DontGenerateMipMaps);
  __label.setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Nearest);
}

void
FirItem::__resetLabel() {
  if (__label.isCreated())
    __label.destroy();
}

void
FirItem::__invalidate() {
  __resetLabel();
}
