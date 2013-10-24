/*
    airport.cpp
    Copyright (C) 2012  Michał Garapich michal@garapich.pl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtGui>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"

#include "glutils/glresourcemanager.h"

#include "ui/widgets/mapwidget.h"

#include "vatsinatorapplication.h"

#include "airport.h"
#include "defines.h"

Airport::Airport(const QString& _icao) :
    __firs{NULL, NULL},
    __data(AirportDatabase::getSingleton().find(_icao)),
    __labelTip(0) {
  if (!__data)
    VatsinatorApplication::log("Airport %s not found!", _icao.toStdString().c_str());
  
  if (__data) {
    __firs[0] = FirDatabase::getSingleton().find(QString(__data->fir_a), __data->is_fir_a_oceanic);
    __firs[1] = FirDatabase::getSingleton().find(QString(__data->fir_b), __data->is_fir_b_oceanic);
  }
}

Airport::Airport(const AirportRecord* _ap) :
     __firs{NULL, NULL},
     __data(_ap),
     __labelTip(0) {
  if (__data) {
    __firs[0] = FirDatabase::getSingleton().find(QString(__data->fir_a), __data->is_fir_a_oceanic);
    __firs[1] = FirDatabase::getSingleton().find(QString(__data->fir_b), __data->is_fir_b_oceanic);
  }
}

Airport::~Airport() {
  if (__labelTip)
    GlResourceManager::deleteImage(__labelTip);
}

GLuint
Airport::__generateTip() const {
  Q_ASSERT(__data);
  
  QImage temp(MapWidget::getSingleton().airportToolTipBackground());
  QPainter painter(&temp);
  painter.setRenderHint(QPainter::TextAntialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.setRenderHint(QPainter::HighQualityAntialiasing);
  painter.setFont(MapWidget::getSingleton().airportFont());
  painter.setPen(MapWidget::getSingleton().airportPen());
  QRect rectangle(8, 2, 48, 12); // size of the tooltip.png
  painter.drawText(rectangle, Qt::AlignCenter, static_cast< QString >(__data->icao));
  __labelTip = GlResourceManager::loadImage(temp);
  return __labelTip;
}