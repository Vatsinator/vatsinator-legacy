/*
 * mapwidget.h
 * Copyright (C) 2013  Michał Garapich <michal@garapich.pl>
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

#include "singleton.h"

class Airport;
class Client;
class Fir;
class FrameBufferObject;
class Pilot;
class WorldPolygon;

class MapWidget : public QGLWidget, public Singleton<MapWidget> {
  
  Q_OBJECT

signals:
  void menuRequest(const Pilot*);
  void menuRequest(const Airport*);
  void menuRequest(const Fir*);
  void menuRequest();
  
  void windowRequest(const Client*);
  void windowRequest(const Airport*);
  void windowRequest(const Fir*);
  
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
  QPointF mapToLonLat(const QPoint&);
  
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
  QPointF scaleToLonLat(const QPoint&);
  
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
  
private:
  /**
   * Renders the texture that is stored in FBO.
   */
  void __renderTexture();
  
  void __drawWorld();
  
  /**
   * Updates framebuffer object. Must be called each window resize.
   * @param width The new width.
   * @param height The new height.
   */
  void __updateFbo(int, int);
  void __updateZoom(int);
  
private slots:
  void __reloadSettings();
  
private:
  FrameBufferObject* __fbo;
  
  /* Global coordinates of the center point of the map */
  QPointF __center;
  
  /* Stores screen rectangle */
  qreal __rangeX;
  qreal __rangeY;
  
  
  /* Zoom Coefficient to let users customize their zooming speed */
  /* Zoom Coefficient is defined in MiscellaneousPage */

  /* Minimum for __actualZoom not to exceed ZOOM_MAXIMUM value */
  int __actualZoomMaximum;
  
  /*Actual Zoom level*/
  int __actualZoom;
  
  /* Zoom factor */
  float   __zoom;
  
  
  QPoint __mousePosition;
  QPoint __lastClickPosition;
  
  /* World map drawer */
  WorldPolygon* __world;
  
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
