/*
 * maprenderer.h
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

#ifndef MAPRENDERER_H
#define MAPRENDERER_H

#include <QObject>
#include <QSize>
#include <QColor>
#include <QMatrix4x4>

#include "ui/map/qtopenglcompat.h"
#include "vatsimdata/lonlat.h"

class IconKeeper;
class MapItem;
class MapScene;
class ModelMatcher;
class WorldPolygon;

/**
 * The MapRenderer class takes care of rendering the map.
 */
class MapRenderer : public QObject {
  Q_OBJECT
  
  /**
   * The zoom property describes the current zoom of the map. The larger the
   * value is, the smaller range is visible.
   */
  Q_PROPERTY(int zoom READ zoom WRITE setZoom)
  
  /**
   * The center property represents the center of the map.
   */
  Q_PROPERTY(LonLat center READ center WRITE setCenter)
  
signals:
  
  /**
   * One or more of the properties have changed and therefore the map needs
   * to be re-rendered.
   */
  void updated();

public:
  
  /**
   * The default constructor requires valid OpenGL context. All initialization
   * goes here.
   */
  explicit MapRenderer(QObject* = 0);
  
  /**
   * The destructor deletes shaders, frees buffers, etc. A valid OpenGL context
   * must be present.
   */
  virtual ~MapRenderer();
  
  /**
   * Gets screen coordinates (0 - winWidth, 0 - winHeight) and
   * maps them to longitude & latitude.
   * 
   * @param point The point on the screen.
   * @return Global coordinates (longitude, latitude).
   */
  LonLat mapToLonLat(const QPoint&);
  
  /**
   * Scales the given point (or distance) to the global
   * coordinates system. It does not differ from the
   * mapToLonLat() function except the fact that it 
   * does not take into consideration the current map
   * center point.
   * 
   * @param point The point (or the distance) in the window coordinates.
   * @return Global coordinates (or the distance).
   */
  LonLat scaleToLonLat(const QPoint&);
  
  /**
   * Calculates widget coordinates from the given lat-lon coordinates.
   */
  QPoint mapFromLonLat(const LonLat&);
  
  /**
   * Calculates OpenGL scene local coordinates from latitude/longitude.
   */
  QPointF glFromLonLat(const LonLat&);
  
  /**
   * Draws the specified item's "under mouse" elements.
   */
  void drawFocused(const MapItem*);
  
  void setZoom(int);
  void setCenter(const LonLat&);
  
  /**
   * The difference between the updateZoom() and setZoom() methods is that
   * updateZoom() updates the zoom property smoothly, i.e. it does some
   * equations that consider the zoom coefficient specified by the user.
   */
  void updateZoom(int);
  
  /**
   * Updates the viewport of the scene.
   */
  void setViewport(const QSize&);
  
  inline int zoom() const { return __zoom; }
  inline const LonLat& center() const { return __center; }
  
  inline int programColorLocation() const { return __identityColorLocation; }
  inline int programZLocation() const { return __texturedZLocation; }
  inline int programRotationLocation() const { return __texturedRotationLocation; }
  inline IconKeeper* icons() { return __iconKeeper; }
  inline ModelMatcher* models() { return __modelMatcher; }
  inline MapScene* scene() { return __scene; }
  inline QOpenGLFunctions* opengl() { return __functions; }
  
  /**
   * Vertex attribute location ("vertex").
   */
  inline static constexpr int vertexLocation() { return 0; }
  
  /**
   * Texture coordinate location ("texcoord").
   */
  inline static constexpr int texcoordLocation() { return 1; }
  
public slots:
  void paint();
  
private:
  void __drawWorld();
  void __drawFirs();
  void __drawUirs();
  void __drawItems();
  
  void __storeSettings();
  void __restoreSettings();
  
  void __createShaderPrograms();
  
  void __updateOffsets();
  void __updateScreen();
  
private:
  
  /* The current viewport */
  QSize __viewport;
  
  /* Geo coordinates of the current viewport */
  QRectF __screen;
  
  /* Keeps items that are currently on the screen */
  QList<const MapItem*> __items;
  
  /* The current zoom property */
  int __zoom;
  
  /* The center of the map */
  LonLat __center;
  
  /* OpenGL functions */
  QOpenGLFunctions* __functions;
  
  /* World map drawer */
  WorldPolygon* __world;
  
  /* The IconKeeper instance */
  IconKeeper* __iconKeeper;
  
  /* The ModelMatcher instance */
  ModelMatcher* __modelMatcher;
  
  /* Scene handler */
  MapScene* __scene;
  
  /* To have the map repeated, we keep offsets */
  QList<float> __offsets;
  
  /* Current offset */
  float __xOffset;
  
  /* Stores screen rectangle */
  float __rangeX;
  float __rangeY;
  
  /* Shader programs */
  QOpenGLShaderProgram* __identityProgram;
  QOpenGLShaderProgram* __texturedProgram;
  
  /* Shader variable locations */
  int __identityMatrixLocation;
  int __identityColorLocation;
  int __identityOffsetLocation;
  int __texturedMatrixLocation;
  int __texturedPositionLocation;
  int __texturedRotationLocation;
  int __texturedZLocation;
  
  /* Projection matrix */
  QMatrix4x4 __projection;
  
  /* Model-View matrices */
  QMatrix4x4 __worldTransform;
  
  /* Zoom Coefficient to let users customize their zooming speed */
  /* Zoom Coefficient is defined in MiscellaneousPage */

  /* Minimum for __actualZoom not to exceed ZOOM_MAXIMUM value */
  int __actualZoomMaximum;
  
  /*Actual Zoom level*/
  int __actualZoom;

};

#endif // MAPRENDERER_H
