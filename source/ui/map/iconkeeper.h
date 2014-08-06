/*
 * iconkeeper.h
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

#ifndef ICONKEEPER_H
#define ICONKEEPER_H

#include <QObject>
#include <QOpenGLTexture>

/**
 * The IconKeeper class is responsible for managing some static icons
 * that are used by the map, such as airport icons. The icons are kept
 * in OpenGL texture format.
 */
class IconKeeper : public QObject {
  Q_OBJECT

public:
  
  explicit IconKeeper(QObject* = nullptr);
  virtual ~IconKeeper();
  
  QOpenGLTexture* emptyAirportIcon();
  QOpenGLTexture* activeAirportIcon();
  QOpenGLTexture* activeStaffedAirportIcon();
  
private:
  QOpenGLTexture __emptyAirportIcon;
  QOpenGLTexture __activeAirportIcon;
  QOpenGLTexture __activeStaffedAirportIcon;
  
};

#endif // ICONKEEPER_H
