/*
 * flightitem.cpp
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

#include "db/airportdatabase.h"
#include "glutils/texture.h"
#include "modules/modelmatcher.h"
#include "storage/settingsmanager.h"
#include "ui/map/mapconfig.h"
#include "ui/map/maprenderer.h"
#include "ui/map/mapscene.h"
#include "ui/userinterface.h"
#include "vatsimdata/client/pilot.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "vatsinatorapplication.h"

#include "flightitem.h"

FlightItem::FlightItem(const Pilot* _pilot, QObject* _parent) :
    QObject(_parent),
    __scene(qobject_cast<MapScene*>(_parent)),
    __pilot(_pilot),
    __position(_pilot->position()),
    __model(nullptr),
    __label(QOpenGLTexture::Target2D),
    __linesReady(false) {
  connect(vApp()->settingsManager(),            SIGNAL(settingsChanged()),
          this,                                 SLOT(__reloadSettings())); 
  connect(_pilot,                               SIGNAL(updated()),
          this,                                 SLOT(__invalidate()));
}

FlightItem::~FlightItem() {
  __label.destroy();
}

void
FlightItem::drawModel(QOpenGLShaderProgram* _shader) const {
  static const GLfloat modelRect[] = {
    -0.03, -0.03,
    -0.03,  0.03,
     0.03,  0.03,
     0.03,  0.03,
     0.03, -0.03,
    -0.03, -0.03
  };
  
  static const GLfloat textureCoords[] = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f
  };
  
  if (!__model)
    __matchModel();
  
  _shader->setAttributeArray(MapRenderer::texcoordLocation(), textureCoords, 2);
  _shader->setAttributeArray(MapRenderer::vertexLocation(), modelRect, 2);
  
//     glRotatef(static_cast<GLfloat>(data()->heading()), 0, 0, -1);
  __model->bind();
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void
FlightItem::drawLabel(QOpenGLShaderProgram* _shader) const {
  static const GLfloat labelRect[] = {
    -0.16,  0.019,
    -0.16,  0.12566666,
     0.16,  0.12566666,
     0.16,  0.12566666,
     0.16,  0.019,
    -0.16,  0.019
  };
  
  static const GLfloat textureCoords[] = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f
  };
  
  _shader->setAttributeArray(MapRenderer::texcoordLocation(), textureCoords, 2);
  _shader->setAttributeArray(MapRenderer::vertexLocation(), labelRect, 2);
  
  if (!__label.isCreated())
    __initializeLabel();
  
  __label.bind();
  glDrawArrays(GL_TRIANGLES, 0, 6);
  __label.release();
}

void
FlightItem::drawLines(LineTypes types, QOpenGLShaderProgram* _shader) const {
  if (!__linesReady)
    __prepareLines();
  
  if (types.testFlag(OriginToPilot) && !__otpLine.coords.isEmpty()) {
    if (!__otpLine.color.isValid())
      __otpLine.color = SM::get("map.origin_to_pilot_line_color").value<QColor>();
    
    _shader->setUniformValue(__scene->renderer()->identityColorLocation(), __otpLine.color);
    _shader->setAttributeArray(MapRenderer::vertexLocation(), __otpLine.coords.constData(), 2);
    glDrawArrays(GL_LINE_STRIP, 0, __otpLine.coords.size() / 2);
  }
  
  if (types.testFlag(PilotToDestination) && !__ptdLine.coords.isEmpty()) {
    if (!__ptdLine.color.isValid())
      __ptdLine.color = SM::get("map.pilot_to_destination_line_color").value<QColor>();
    
    _shader->setUniformValue(__scene->renderer()->identityColorLocation(), __ptdLine.color);
    _shader->setAttributeArray(MapRenderer::vertexLocation(), __ptdLine.coords.constData(), 2);
    glLineStipple(3, 0xF0F0); // dashed line
    glDrawArrays(GL_LINE_STRIP, 0, __ptdLine.coords.size() / 2);
    glLineStipple(1, 0xFFFF);
  }
}

bool
FlightItem::needsDrawing() const {
  return !(data()->phase() != Pilot::Airborne || data()->isPrefiledOnly());
}

const LonLat &
FlightItem::position() const {
  return __position;
}

QString
FlightItem::tooltipText() const {
  QString callsign = data()->callsign();
  QString desc = QString("%1 (%2)").arg(data()->realName(), data()->aircraft());
  
  QString from;
  const Airport* ap = data()->origin();
  if (ap)
    from = QString(ap->icao()) % QString(" ") % QString::fromUtf8(ap->data()->city);
  else
    from = tr("(unknown)");
  
  QString to;
  ap = data()->destination();
  if (ap)
    to = QString(ap->icao()) % QString(" ") % QString::fromUtf8(ap->data()->city);
  else
    to = tr("(unknown)");
  
  QString gs = tr("Ground speed: %1 kts").arg(QString::number(data()->groundSpeed()));
  QString alt = tr("Altitude: %1 ft").arg(QString::number(data()->altitude()));
  
  return QString("<p style='white-space:nowrap'><center>"
    % callsign % "<br />"
    % desc % "<br />"
    % from % " > " % to % "<br />"
    % gs % "<br />"
    % alt
    % "</center></p>");
}

void
FlightItem::showDetails() const {
  vApp()->userInterface()->showDetails(data());
}

void
FlightItem::__initializeLabel() const {
  static QRect labelRect(28, 10, 73, 13);
  
  if (__label.isCreated())
    __label.destroy();
  
  QString callsign(data()->callsign());
  
  QImage temp(MapConfig::pilotLabelBackground());
  QPainter painter(&temp);
  painter.setRenderHint(QPainter::TextAntialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.setRenderHint(QPainter::HighQualityAntialiasing);
  
  painter.setFont(SM::get("map.pilot_font").value<QFont>());
  painter.setPen(MapConfig::pilotPen());
  
  painter.drawText(labelRect, Qt::AlignCenter, callsign);
  __label.setData(temp.mirrored(), QOpenGLTexture::DontGenerateMipMaps);
  __label.setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Nearest);
}

void
FlightItem::__prepareLines() const {
  QVector<GLfloat>* coords = &__otpLine.coords;
  for (const LonLat& p: data()->route().waypoints) {
    (*coords) << p.x() << p.y();
    if (p == data()->position()) {
      coords = &__ptdLine.coords;
      (*coords) << p.x() << p.y();
    }
  }
  
  __linesReady = true;
}

void
FlightItem::__matchModel() const {
  __model = ModelMatcher::getSingleton().matchMyModel(__pilot->aircraft());
}

void
FlightItem::__reloadSettings() {
  if (__label.isCreated()) {
    __label.destroy();
  }
  
  __otpLine.color = QColor();
  __ptdLine.color = QColor();
}

void
FlightItem::__invalidate() {
  __position = __pilot->position();
  
  __model = nullptr;
  
  __linesReady = false;
  __otpLine.coords.clear();
  __ptdLine.coords.clear();
 
  /* We don't need to invalidate the label - pilot can't just change his
   * callsign, can he?
   */
}
