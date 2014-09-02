/*
 * iconkeeper.cpp
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

#include "ui/map/mapconfig.h"

#include "iconkeeper.h"

IconKeeper::IconKeeper(QObject* _parent) :
    QObject(_parent),
    __emptyAirportIcon(QOpenGLTexture::Target2D),
    __activeAirportIcon(QOpenGLTexture::Target2D),
    __activeStaffedAirportIcon(QOpenGLTexture::Target2D) {}

IconKeeper::~IconKeeper() {
  __emptyAirportIcon.destroy();
  __activeAirportIcon.destroy();
  __activeStaffedAirportIcon.destroy();
}

QOpenGLTexture*
IconKeeper::emptyAirportIcon() {
  if (!__emptyAirportIcon.isCreated()) {
    __emptyAirportIcon.setData(QImage(MapConfig::emptyAirportIcon()).mirrored(), QOpenGLTexture::DontGenerateMipMaps);
    __emptyAirportIcon.setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Nearest);
  }
  return &__emptyAirportIcon;
}

QOpenGLTexture*
IconKeeper::activeAirportIcon() {
  if (!__activeAirportIcon.isCreated()) {
    __activeAirportIcon.setData(QImage(MapConfig::activeAirportIcon()).mirrored(), QOpenGLTexture::DontGenerateMipMaps);
    __activeAirportIcon.setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Nearest);
  }
  return &__activeAirportIcon;
}

QOpenGLTexture*
IconKeeper::activeStaffedAirportIcon() {
  if (!__activeStaffedAirportIcon.isCreated()) {
    __activeStaffedAirportIcon.setData(QImage(MapConfig::activeStaffedAirportIcon()).mirrored(), QOpenGLTexture::DontGenerateMipMaps);
    __activeStaffedAirportIcon.setMinMagFilters(QOpenGLTexture::Linear, QOpenGLTexture::Nearest);
  }
  return &__activeStaffedAirportIcon;
}
