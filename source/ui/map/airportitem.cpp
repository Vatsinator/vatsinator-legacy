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
#include "glutils/glresourcemanager.h"
#include "storage/settingsmanager.h"
#include "ui/actions/actionmenuseparator.h"
#include "ui/actions/airportdetailsaction.h"
#include "ui/actions/clientdetailsaction.h"
#include "ui/actions/metaraction.h"
#include "ui/map/mapconfig.h"
#include "ui/windows/airportdetailswindow.h"
#include "ui/windows/atcdetailswindow.h"
#include "ui/windows/metarswindow.h"
#include "vatsimdata/airport.h"
#include "vatsimdata/airport/activeairport.h"
#include "vatsimdata/airport/emptyairport.h"
#include "vatsimdata/models/controllertablemodel.h"

#include "airportitem.h"
#include "defines.h"

AirportItem::AirportItem(const Airport* _ap, QObject* _parent) :
    QObject(_parent),
    __airport(_ap),
    __position(_ap->data()->longitude, _ap->data()->latitude),
    __icon(0),
    __label(0) {
  
  connect(SettingsManager::getSingletonPtr(),   SIGNAL(settingsChanged()),
          this,                                 SLOT(__resetLabel()));
}

AirportItem::~AirportItem() {
  if (__label)
    GlResourceManager::deleteImage(__label);
}

void
AirportItem::drawIcon() const {
  static const GLfloat iconRect[] = {
    -0.04, -0.02,
    -0.04,  0.06,
     0.04,  0.06,
     0.04, -0.02
  };
  
  if (!__icon)
    __makeIcon();
  
  glBindTexture(GL_TEXTURE_2D, __icon);
  glVertexPointer(2, GL_FLOAT, 0, iconRect);
  glDrawArrays(GL_QUADS, 0, 4);
  glBindTexture(GL_TEXTURE_2D, 0);
}

bool
AirportItem::needsDrawing() const {
  return !__position.isNull();
}

void
AirportItem::drawLabel() const {
  static const GLfloat labelRect[] = {
    -0.08, -0.05333333,
    -0.08,  0,
     0.08,  0,
     0.08, -0.05333333
  };
  
  if (position().isNull())
    return;
  
  if (!__label)
    __generateLabel();
  
  glBindTexture(GL_TEXTURE_2D, __label);
  glVertexPointer(2, GL_FLOAT, 0, labelRect);
  glDrawArrays(GL_QUADS, 0, 4);
  glBindTexture(GL_TEXTURE_2D, 0);
}

const QPointF &
AirportItem::position() const {
  return __position;
}

QString
AirportItem::tooltipText() const {
  QString desc = QString("%1 %2, %3").arg(
    data()->data()->icao,
    QString::fromUtf8(data()->data()->name),
    QString::fromUtf8(data()->data()->city));
  
  QString staff, deparr;
  const ActiveAirport* a = dynamic_cast<const ActiveAirport*>(data());
  if (a) {
    for (const Controller* c: a->staffModel()->staff()) {
      staff.append("<br>");
      staff.append(QString("%1 %2 %3").arg(c->callsign(), c->frequency(), c->realName()));
    }
    
    int deps = a->countDepartures();
    if (deps > 0) {
      deparr.append("<br>");
      deparr.append(tr("Departures: %1").arg(QString::number(deps)));
    }
    
    int arrs = a->countArrivals();
    if (arrs > 0) {
      deparr.append("<br>");
      deparr.append(tr("Arrivals: %1").arg(QString::number(arrs)));
    }
  }
  
  return QString("<p style='white-space:nowrap'><center>" % desc % staff % deparr % "</center></p>");
}

QMenu *
AirportItem::menu(QWidget* _parent) const {
  QMenu* menu = new QMenu(data()->data()->icao, _parent);
  
  AirportDetailsAction* showAp = new AirportDetailsAction(data(), tr("Airport details"), _parent);
  connect(showAp,                                       SIGNAL(triggered(const Airport*)),
          AirportDetailsWindow::getSingletonPtr(),      SLOT(show(const Airport*)));
  menu->addAction(showAp);
  
  MetarAction* showMetar = new MetarAction(data()->data()->icao, _parent);
  connect(showMetar,                                    SIGNAL(triggered(QString)),
          MetarsWindow::getSingletonPtr(),              SLOT(show(QString)));
  menu->addAction(showMetar);
  
  const ActiveAirport* a = dynamic_cast<const ActiveAirport*>(data());
  if (a) {
    if (!a->staffModel()->staff().isEmpty()) {
      menu->addSeparator();
      menu->addAction(new ActionMenuSeparator(tr("Controllers"), _parent));
      
      for (const Controller* c: a->staffModel()->staff()) {
        ClientDetailsAction* cda = new ClientDetailsAction(c, c->callsign(), _parent);
        connect(cda,                                    SIGNAL(triggered(const Client*)),
                AtcDetailsWindow::getSingletonPtr(),    SLOT(show(const Client*)));
        menu->addAction(cda);
      }
    }
  }
  
  return menu;
}

void
AirportItem::showDetailsWindow() const {
  AirportDetailsWindow::getSingletonPtr()->show(data());
}

void
AirportItem::__makeIcon() const {
  const ActiveAirport* a = dynamic_cast<const ActiveAirport*>(data());
  if (a) {
    if (a->staffModel()->staff().isEmpty()) {
      __icon = __icons.activeAirportIcon();
    } else {
      __icon = __icons.activeStaffedAirportIcon();
    }
  } else {
    __icon = __icons.emptyAirportIcon();
  }
}

void
AirportItem::__generateLabel() const {
  static QRect labelRect(8, 2, 48, 12);
  
  if (__label)
    GlResourceManager::deleteImage(__label);
  
  QString icao(data()->data()->icao);
  
  QImage temp(MapConfig::airportLabelBackground());
  QPainter painter(&temp);
  painter.setRenderHint(QPainter::TextAntialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.setRenderHint(QPainter::HighQualityAntialiasing);
  
  painter.setFont(SM::get("map.airport_font").value<QFont>());
  painter.setPen(MapConfig::airportPen());
  
  painter.drawText(labelRect, Qt::AlignCenter, icao);
  __label = GlResourceManager::loadImage(temp);
}

void
AirportItem::__resetLabel() {
  if (__label) {
    GlResourceManager::deleteImage(__label);
    __label = 0;
  }
}

AirportItem::IconKeeper::IconKeeper() :
    __emptyAirportIcon(0),
    __activeAirportIcon(0),
    __activeStaffedAirportIcon(0) {}

AirportItem::IconKeeper::~IconKeeper() {
  if (__emptyAirportIcon)
    GlResourceManager::deleteImage(__emptyAirportIcon);
  
  if (__activeAirportIcon)
    GlResourceManager::deleteImage(__activeAirportIcon);
  
  if (__activeStaffedAirportIcon)
    GlResourceManager::deleteImage(__activeStaffedAirportIcon);
}

GLuint
AirportItem::IconKeeper::emptyAirportIcon() {
  if (!__emptyAirportIcon)
    __emptyAirportIcon = GlResourceManager::loadImage(MapConfig::emptyAirportIcon());
  
  return __emptyAirportIcon;
}

GLuint
AirportItem::IconKeeper::activeAirportIcon() {
  if (!__activeAirportIcon)
    __activeAirportIcon = GlResourceManager::loadImage(MapConfig::activeAirportIcon());
  
  return __activeAirportIcon;
}

GLuint AirportItem::IconKeeper::activeStaffedAirportIcon() {
  if (!__activeStaffedAirportIcon)
    __activeStaffedAirportIcon = GlResourceManager::loadImage(MapConfig::activeStaffedAirportIcon());
  
  return __activeStaffedAirportIcon;
}

AirportItem::IconKeeper AirportItem::__icons;
