/*
 * airportitem.cpp
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

#include "db/airportdatabase.h"
#include "storage/settingsmanager.h"
#include "ui/map/approachcircleitem.h"
#include "ui/map/iconkeeper.h"
#include "ui/map/mapconfig.h"
#include "ui/map/maprenderer.h"
#include "ui/map/mapscene.h"
#include "ui/userinterface.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"
#include "vatsinatorapplication.h"

#include "airportitem.h"

AirportItem::AirportItem(const Airport* airport, QObject* parent) :
    QObject(parent),
    __scene(qobject_cast<MapScene*>(parent)),
    __airport(airport),
    __position(airport->data()->longitude, airport->data()->latitude),
    __approachCircle(nullptr),
    __icon(nullptr),
    __label(QOpenGLTexture::Target2D),
    __linesReady(false) {
  
  connect(vApp()->settingsManager(),            SIGNAL(settingsChanged()),
          this,                                 SLOT(__reloadSettings()));
  connect(__airport,                            SIGNAL(updated()),
          this,                                 SLOT(__invalidate()));
}

AirportItem::~AirportItem() {
  __label.destroy();
}

bool
AirportItem::isVisible() const {
  Q_ASSERT(!__position.isNull());
  
  if (data()->isEmpty())
    return __scene->settings().view.empty_airports;
  else
    return __scene->settings().view.airports_layer;
}

bool
AirportItem::isLabelVisible() const {
  return __scene->settings().view.airport_labels;
}

const LonLat &
AirportItem::position() const {
  return __position;
}

void
AirportItem::drawItem(QOpenGLShaderProgram* shader) const {
  static Q_DECL_CONSTEXPR float ActiveAirportsZ = static_cast<float>(MapConfig::MapLayers::ActiveAirports);
  
  static const GLfloat iconRect[] = {
    -0.04f, -0.02f,
    -0.04f,  0.06f,
     0.04f,  0.06f,
     0.04f,  0.06f,
     0.04f, -0.02f,
    -0.04f, -0.02f
  };
  
  static const GLfloat textureCoords[] = {
    0.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f
  };
  
  if (!__icon)
    __takeIcon();
  
  shader->setAttributeArray(MapRenderer::texcoordLocation(), textureCoords, 2);
  shader->setAttributeArray(MapRenderer::vertexLocation(), iconRect, 2);
  shader->setUniformValue(__scene->renderer()->programZLocation(), ActiveAirportsZ);
  
  __icon->bind();
  glDrawArrays(GL_TRIANGLES, 0, 6);
//   __icon->release();
}

void
AirportItem::drawLabel(QOpenGLShaderProgram* shader) const {
  static const GLfloat labelRect[] = {
    -0.08f, -0.05333333f,
    -0.08f,  0.0f,
     0.08f,  0.0f,
     0.08f,  0.0f,
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
  
  if (data()->isEmpty())
    return;
  
  shader->setAttributeArray(MapRenderer::texcoordLocation(), textureCoords, 2);
  shader->setAttributeArray(MapRenderer::vertexLocation(), labelRect, 2);
  
  if (!__label.isCreated())
    __initializeLabel();
  
  __label.bind();
  glDrawArrays(GL_TRIANGLES, 0, 6);
  __label.release();
}

void
AirportItem::drawFocused(QOpenGLShaderProgram* shader) const {
  if (!__linesReady)
    __prepareLines();
  
  if (!__otpLines.color.isValid())
      __otpLines.color = SM::get("map.origin_to_pilot_line_color").value<QColor>();

  shader->setUniformValue(__scene->renderer()->programColorLocation(), __otpLines.color);
  shader->setAttributeArray(MapRenderer::vertexLocation(), __otpLines.coords.constData(), 2);
  glDrawArrays(GL_LINES, 0, __otpLines.coords.size() / 2);
  
  if (!__ptdLines.color.isValid())
    __ptdLines.color = SM::get("map.pilot_to_destination_line_color").value<QColor>();
  
  shader->setUniformValue(__scene->renderer()->programColorLocation(), __ptdLines.color);  
  shader->setAttributeArray(MapRenderer::vertexLocation(), __ptdLines.coords.constData(), 2);
  glLineStipple(3, 0xF0F0); // dashed line
  glDrawArrays(GL_LINE_STRIP, 0, __ptdLines.coords.size() / 2);
  glLineStipple(1, 0xFFFF);
}

QString
AirportItem::tooltipText() const {
  QString desc = QString("%1 %2, %3").arg(
    data()->data()->icao,
    QString::fromUtf8(data()->data()->name),
    QString::fromUtf8(data()->data()->city));
  
  QString staff, deparr;
  if (!data()->isEmpty()) {
    for (const Controller* c: data()->staff()->staff()) {
      staff.append("<br>");
      staff.append(QString("%1 %2 %3").arg(c->callsign(), c->frequency(), c->realName()));
    }
    
    int deps = data()->countDepartures();
    if (deps > 0) {
      deparr.append("<br>");
      deparr.append(tr("Departures: %1").arg(QString::number(deps)));
    }
    
    int arrs = data()->countArrivals();
    if (arrs > 0) {
      deparr.append("<br>");
      deparr.append(tr("Arrivals: %1").arg(QString::number(arrs)));
    }
  }
  
  return QString("<p style='white-space:nowrap'><center>" % desc % staff % deparr % "</center></p>");
}

void
AirportItem::showDetails() const {
  vApp()->userInterface()->showDetails(data());
}

void
AirportItem::__takeIcon() const {
  if (data()->isEmpty()) {
    __icon = __scene->renderer()->icons()->emptyAirportIcon();
  } else {
    if (data()->staff()->staff().isEmpty()) {
      __icon = __scene->renderer()->icons()->activeAirportIcon();
    } else {
      __icon = __scene->renderer()->icons()->activeStaffedAirportIcon();
    }
  }
}

void
AirportItem::__prepareLines() const {
  for (const Pilot* p: data()->outbounds()->flights()) {
    for (const LonLat& ll: p->route().waypoints) {
      __otpLines.coords << ll.x() << ll.y();
      if (ll == p->position())
        break;
      else if (ll != p->route().waypoints[0])
        __otpLines.coords << ll.x() << ll.y();
    }
  }
  
  for (const Pilot* p: data()->inbounds()->flights()) {
    bool append = false;
    for (const LonLat& ll: p->route().waypoints) {
      if (append)
        __ptdLines.coords << ll.x() << ll.y();
      if (ll == p->position())
        append = true;
      if (append && ll != p->route().waypoints.last())
        __ptdLines.coords << ll.x() << ll.y();
    }
  }
  
  
  
  __linesReady = true;
}

void
AirportItem::__initializeLabel() const {
  static QRect labelRect(8, 2, 48, 12);
  
  if (__label.isCreated())
    __label.destroy();
  
  QString icao(data()->data()->icao);
  
  QImage temp(MapConfig::airportLabelBackground());
  QPainter painter(&temp);
  painter.setRenderHint(QPainter::TextAntialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.setRenderHint(QPainter::HighQualityAntialiasing);
  
  painter.setFont(SM::get("map.airport_font").value<QFont>());
  painter.setPen(MapConfig::airportPen());
  
  painter.drawText(labelRect, Qt::AlignCenter, icao);
  __label.setData(temp.mirrored(), QOpenGLTexture::DontGenerateMipMaps);
  __label.setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Nearest);
}

void
AirportItem::__reloadSettings() {
  if (__label.isCreated())
    __label.destroy();
}

void
AirportItem::__invalidate() {
  __icon = nullptr;
  
  if (__airport->facilities().testFlag(Controller::App)) {
    if (!__approachCircle)
      __approachCircle = new ApproachCircleItem(data(), this);
  } else {
    if (__approachCircle) {
      __approachCircle->deleteLater();
      __approachCircle = nullptr;
    }
  }
  
  __linesReady = false;
  __otpLines.coords.clear();
  __ptdLines.coords.clear();
}
