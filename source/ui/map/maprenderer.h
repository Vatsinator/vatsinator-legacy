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
#include "vatsimdata/lonlat.h"

class IconKeeper;
class MapItem;
class MapScene;
class WorldPolygon;
class QOpenGLShaderProgram;

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
   * Emited when one of the properties change and therefore the map needs to
   * be redrawn.
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
   * Specifies whether the given point is visible on the screen or not.
   */
  bool onScreen(const QPointF&);
  
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
  
  inline int identityColorLocation() const { return __identityColorLocation; }
  inline IconKeeper* icons() { return __iconKeeper; }
  inline MapScene* scene() { return __scene; }
  
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
  void __drawAirports();
  void __drawPilots();
  void __drawLines();
  
  void __storeSettings();
  void __restoreSettings();
  
  void __createShaderPrograms();
  
  void __updateOffsets();
  
private slots:
  void __reloadSettings();
  
private:
  
  /**
   * Based on user settings, checks whether the pilot's label should
   * be drawn or not.
   */
  bool __shouldDrawPilotLabel(const MapItem*);
  
  /* The current viewport */
  QSize __viewport;
  
  /* The current zoom property */
  int __zoom;
  
  /* The center of the map */
  LonLat __center;
  
  /* World map drawer */
  WorldPolygon* __world;
  
  /* The IconKeeper instance */
  IconKeeper* __iconKeeper;
  
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
  
  
  /* Structs below store settings locally to avoid expensive SM::get() calls */
  struct {
    struct {
      int zoom_coefficient;
    } misc;
    
    struct {
      QColor lands;
      QColor seas;
      QColor staffed_fir_borders;
      QColor staffed_fir_background;
      QColor staffed_uir_borders;
      QColor staffed_uir_background;
      QColor unstaffed_fir_borders;
      QColor approach_circle;
    } colors;
    
    struct {
      bool airports_layer;
      bool airport_labels;
      bool pilots_layer;
      bool staffed_firs;
      bool unstaffed_firs;
      bool empty_airports;
      
      struct {
        bool always;
        bool airport_related;
        bool when_hovered;
      } pilot_labels;
    } view;
  } __settings;

};

#endif // MAPRENDERER_H
