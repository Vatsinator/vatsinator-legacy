/*
    mapwidget.h
    Copyright (C) 2012-2013  Michał Garapich michal@garapich.pl
    Copyright (C) 2012       Jan Macheta janmacheta@gmail.com

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


#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QGLWidget>
#include <QMenu>
#include <QLabel>

#include "db/airportdatabase.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "singleton.h"

class AirportDetailsWindow;
class AtcDetailsWindow;
class Clickable;
class Client;
class Fir;
class FirDatabase;
class FirDetailsWindow;
class FlightDetailsWindow;
class FlightTracker;
class MetarsWindow;
class Pilot;
class VatsinatorApplication;
class WorldMap;

class MapWidget :
    public QGLWidget,
    public Singleton<MapWidget> {

  /*
   * We need this class to build our QGLWidget for UserInterface class.
   */

  Q_OBJECT

signals:
  /**
   * Emitted when user clicks right mouse button on
   * any pilot.
   * @param pilot The pilot that user clicked on.
   */
  void contextMenuRequested(const Pilot*);
  
  /**
   * Emitted when user clicks right mouse button on
   * any airport.
   * @param airport The airport that user clicked on.
   */
  void contextMenuRequested(const Airport*);
  
  /**
   * Emitted when user clicks right mouse button on
   * any FIR.
   * @param fir The FIR that user clicked.
   */
  void contextMenuRequested(const Fir*);
  
  /**
   * Emitted when user clicks right mouse button in the middle
   * of nowhere.
   */
  void contextMenuRequested();
  
  /**
   * Emitted when user clicks left mouse button on
   * any pilot.
   * @param pilot The pilot that user clicked.
   */
  void flightDetailsWindowRequested(const Client*);
  
  /**
   * Emitted when user clicks left mouse button on
   * any airport.
   * @param airport The airport that user clicked.
   */
  void airportDetailsWindowRequested(const Airport*);
  
  /**
   * Emitted when user clicks left mouse button on
   * any FIR.
   * @param fir The FIR that user clicked.
   */
  void firDetailsWindowRequested(const Fir*);
  
  /**
   * Emitted when user wants to track any plane.
   * @param pilot Plane that user wants to track.
   */
  void flightTrackingRequested(const Pilot*);
  
  /**
   * Emitted when user cancels the flight tracking,
   * either by deselecting the check-box or by moving
   * the map with mouse.
   */
  void flightTrackingCanceled();
  
  /**
   * Called when user toggles "Show airport inbound/outbound
   * lines".
   * @param airport The airport that action was taken on.
   */
  void airportLinesToggled(const Airport*);
  
  /**
   * Called from resizeEvent().
   */
  void resized();

public:
  MapWidget(QWidget* = 0);

  virtual ~MapWidget();
  
  /**
   * Gets latitude and longitude of mouse position on the map.
   * @param lat Stores latitude.
   * @param lon Stores longitude.
   */
  void mouse2LatLon(qreal*, qreal*);
  
  /**
   * Obtains default format, tweaks it and returns.
   */
  static QGLFormat getFormat();
  
  /**
   * Color of font on pilots' tooltips.
   */
  const QColor& pilotPen() const;
  
  /**
   * Color of font on airports' tooltips. 
   */
  const QColor& airportPen() const;
  
  /**
   * Color of font on firs' tooltips.
   */
  const QColor& firPen() const;
  
  /* For Pilot class */
  inline const QImage &
  pilotToolTipBackground() const { return __pilotToolTip; }

  inline const QFont &
  pilotFont() const { return __pilotFont; }

  /* For Airport class */
  inline const QImage &
  airportToolTipBackground() const { return __airportToolTip; }

  inline const QFont &
  airportFont() const { return __airportFont; }

  /* For Fir class */
  inline const QImage &
  firToolTipBackground() const { return __firToolTip; }

  inline const QFont &
  firFont() const { return __firFont; }

  inline bool
  isInitialized() const { return __isInitialized; }

public slots:
  /**
   * Sets the plane at the middle of the map.
   * @param client Client to be shown.
   */
  void showClient(const Client*);
  
  /**
   * Sets the airport at the middle of the map.
   * @param airport Airport to be shown.
   */
  void showAirport(const Airport*);
  
  /**
   * Sets the given point at the middle of the map.
   * @param p Point (lat, lon).
   */
  void showPoint(const QPointF&);
  
  /**
   * Hides the menu (if any), hides the tooltip
   * and redraws the map.
   */
  void redraw();

protected:
  /* Here we reimplement some functions that are needed for
   * OpenGL to work properly. */
  void initializeGL();
  void paintGL();
  void resizeGL(int, int);
  void wheelEvent(QWheelEvent*);
  void mousePressEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);
  void keyPressEvent(QKeyEvent*);
  void keyReleaseEvent(QKeyEvent*);

private slots:
  void __loadNewSettings();
  void __openContextMenu(const Pilot*);
  void __openContextMenu(const Airport*);
  void __openContextMenu(const Fir*);
  void __openContextMenu();
  void __slotUiCreated();

private:
  enum PMMatrixMode {
    /* Describes Projection Matrix mode */
    AIRPORTS_PILOTS, WORLD
  };

  void __init();
  void __prepareMatrix(PMMatrixMode, double = 0.0);

#ifndef NO_DEBUG
  void __drawMarks();
#endif

  void __drawWorld(double = 0.0);
  void __drawFirs(double = 0.0);
  void __drawUirs(double);
  void __drawFirsLabels(float = 0.0);
  void __drawAirports(float = 0.0);
  void __drawPilots(float = 0.0);
  void __drawLines(double = 0.0); // lines when airport/pilot on hover
  void __drawToolTip();

  void __setAntyaliasing(bool);

  void __storeSettings();
  void __restoreSettings();
  void __updateZoom(int);

  void __produceCircle();

  /* Some inlined helpful functions */
  float __distanceFromCamera(float, float);
  void  __mapCoordinates(float, float, float*, float*);
  QString __producePilotToolTip(const Pilot*);
  QString __produceAirportToolTip(const Airport*);
  QString __produceFirToolTip(const Fir*);
  void  __drawCallsign(const Pilot*);
  void  __drawCallsign(GLfloat, GLfloat, const Pilot*);
  void  __drawIcaoLabel(const Airport*);
  void  __drawFirLabel(GLfloat, GLfloat, const Fir&);

  bool  __isInitialized;

  /* OpenGL's textures. */
  GLuint  __apIcon;
  GLuint  __apStaffedIcon;
  GLuint  __apInactiveIcon;

  /* Used by Pilot class */
  QImage  __pilotToolTip;
  QFont   __pilotFont;

  /* For Airport class */
  QImage  __airportToolTip;
  QFont   __airportFont;

  /* For Fir class */
  QImage  __firToolTip;
  QFont   __firFont;

  /* Approach circle array and vertices count */
  GLfloat*  __circle;
  unsigned  __circleCount;

  /* Camera position x, y */
  QPointF   __position;
  
  /*----------ZOOM HANDLING SECTION---------*/
  /* Zoom factor */
  float   __zoom;

  /*Actual Zoom level*/
  int   __actualZoom;

  /* Zoom Coefficient to let users customize their zooming speed */
  /* Zoom Coefficient is defined in MiscellaneousPage */

  /* Minimum for __actualZoom not to exceed ZOOM_MAXIMUM value */
  int  __actualZoomMaximum;
  
  /*----------ZOOM HANDLING SECTION END---------*/
  
  /* Last mouse position */
  QPoint  __lastMousePos;
  QPoint  __recentlyClickedMousePos;

  /* And last mouse position interpolated to ortho range */
  QPointF __lastMousePosInterpolated;

  /* Showing pilots' labels or not? */
  bool  __keyPressed;

  /* Stores window geometry */
  int __winWidth;
  int __winHeight;

  /* Store glOrtho ranges */
  GLdouble  __orthoRangeX;
  GLdouble  __orthoRangeY;

  /* Clickable object under mouse pointer */
  const Clickable*  __underMouse;

  /* To prevent the tooltip from being displayed in wrong moment */
  bool  __contextMenuOpened;

  /* Where to display latitude and longitude of mouse position */
  QLabel*   __label;

  /* Current context menu */
  QMenu*    __menu;

  /* Draw the map copy on the left or right */
  bool    __drawLeft;
  bool    __drawRight;
  double  __360degreesMapped;

  /* We keep some singleton pointers to have the code clean */
  VatsimDataHandler&        __data;
  const AirportMap&        __airports;
  
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
