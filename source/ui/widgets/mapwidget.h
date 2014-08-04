/*
 * mapwidget.h
 * Copyright (C) 2013-2014  Michał Garapich <michal@garapich.pl>
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

#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QGLWidget>
#include <QMatrix4x4>

#include "ui/map/mapstate.h"
#include "vatsimdata/lonlat.h"
#include "singleton.h"

class QOpenGLShaderProgram;

class Airport;
class Client;
class Fir;
class MapEvent;
class MapItem;
class MapScene;
class Pilot;
class WorldPolygon;

class MapWidget : public QGLWidget, public Singleton<MapWidget> {
  
  Q_OBJECT
  
  friend class MapScene;

signals:
  void glReady();
  
  void menuRequest(const MapItem*);
  void menuRequest();
  
  void windowRequest(const MapItem*);
  
  void flightTrackingRequest(const Pilot*);
  void flightTrackingCanceled();
  
  void airportLinesToggled(const Airport*);
  
public:
  explicit MapWidget(QWidget* = 0);
  virtual ~MapWidget();
  
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
  
  /**
   * Captures map events.
   */
  bool event(QEvent*) override;
  
  inline int vertexLocation() const { return __vertexLocation; }
  inline MapScene* scene() { return __scene; }
  inline const MapState& state() const { return __state; }
  
public slots:
  
  /**
   * Closes all the menus, hides tooltip and updates.
   */
  void redraw();
  
protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int, int) override;
  
  void wheelEvent(QWheelEvent*) override;
  void mousePressEvent(QMouseEvent*) override;
  void mouseReleaseEvent(QMouseEvent*) override;
  void mouseMoveEvent(QMouseEvent*) override;
  
  /**
   * Event fired when the underlying scene decides to change the map state.
   */
  bool stateChangeEvent(MapEvent*);
  
private:
  void __drawWorld();
  void __drawFirs();
  void __drawUirs();
  void __drawAirports();
  void __drawPilots();
  void __drawLines();
  
  void __storeSettings();
  void __restoreSettings();
  
  void __updateOffsets();
  
  /**
   * Updates the zoom factor.
   */
  void __updateZoom(int);
  
  /**
   * Shows or hides the tooltip.
   */
  void __updateTooltip();
  
  /**
   * Checks whether the item is under the mouse.
   * If it is, it is handled properly.
   */
  void __checkItem(const MapItem*);
  
  /**
   * Based on user settings, checks whether the pilot's label should
   * be drawn or not.
   */
  bool __shouldDrawPilotLabel(const MapItem*);
  
private slots:
  void __reloadSettings();
  void __showMenu(const MapItem*);
  void __showMenu();
  void __showWindow(const MapItem*);
  
private:
  
  /* Shader program used to render everything */
  QOpenGLShaderProgram* __program;
  
  /* Shader variable locations */
  int __vertexLocation;
  int __matrixLocation;
  int __colorLocation;
  
  /* Projection matrix */
  QMatrix4x4 __projection;
  
  /* Model-View matrices */
  QMatrix4x4 __worldTransform;
  
  /* To have the map repeated, we keep offsets */
  QList<GLfloat> __offsets;
  
  /* Current offset */
  GLfloat __xOffset;
  
  /* Stores screen rectangle */
  qreal __rangeX;
  qreal __rangeY;
  
  
  /* Zoom Coefficient to let users customize their zooming speed */
  /* Zoom Coefficient is defined in MiscellaneousPage */

  /* Minimum for __actualZoom not to exceed ZOOM_MAXIMUM value */
  int __actualZoomMaximum;
  
  /*Actual Zoom level*/
  int __actualZoom;
  
  /* Current map state */
  MapState __state;
  
  /**
   * This class handles mouse position on the screen.
   */
  class MousePosition {
  public:
    MousePosition();
    
    /**
     * Updates the mouse position from location on the widget (as
     * it is given by Qt).
     */
    void update(const QPoint&);
    
    /**
     * Quickly calculates distance between the given point and mouse cursor
     * position on the screen, _in pixels_.
     */
    qreal screenDistance(const QPoint&);
    
    /**
     * Calculates distance between the point on the globe (lat-lon) and
     * the mouse cursor position.
     */
    qreal geoDistance(const LonLat&);
    
    /**
     * Sets the mouse down status.
     */
    void setDown(bool);
    
    /**
     * Position within the widget.
     */
    inline const QPoint& screenPosition() const { return __screenPosition; }
    
    /**
     * Latitude/longitude that the mouse cursor currently points at.
     */
    inline const QPointF& geoPosition() const { return __geoPosition; }
    
    /**
     * Indicates whether any of the mouse buttons is pressed or not.
     */
    inline bool down() const { return __down; }
    
  private:
    QPoint  __screenPosition;
    LonLat  __geoPosition;
    bool    __down;
  } __mousePosition;
  
  QPoint __lastClickPosition;
  const MapItem* __underMouse;
  
  /* World map drawer */
  WorldPolygon* __world;
  
  /* Scene handler */
  MapScene* __scene;
  
  /* Structs below store settings locally to avoid expensive SM::get() calling. */
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

#endif // MAPWIDGET_H
