/*
    mapwidget.h
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


#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QGLWidget>
#include <QMenu>
#include <QLabel>

#include "db/airportdatabase.h"
#include "vatsimdata/pilot.h"
#include "vatsimdata/vatsimdatahandler.h"
#include "singleton.h"

class AirportDetailsWindow;
class ATCDetailsWindow;
class Clickable;
class Fir;
class FirDatabase;
class FirDetailsWindow;
class FlightDetailsWindow;
class FlightTracker;
class MetarsWindow;
class SettingsManager;
class VatsinatorApplication;
class WorldMap;

class MapWidget :
    public QGLWidget,
    public Singleton< MapWidget > {

  /*
   * We need this class to build our QGLWidget for UserInterface class.
   */

  Q_OBJECT

public:
  MapWidget(QWidget* = 0);

  virtual ~MapWidget();

  /* For Pilot class */
  inline const QImage&
  getPilotToolTipBackground() { return __pilotToolTip; }

  inline const QFont&
  getPilotFont() { return __pilotFont; }

  /* For Airport class */
  inline const QImage&
  getAirportToolTipBackground() { return __airportToolTip; }

  inline const QFont&
  getAirportFont() { return __airportFont; }

  /* For Fir class */
  inline const QImage&
  getFirToolTipBackground() { return __firToolTip; }

  inline const QFont&
  getFirFont() { return __firFont; }

  inline bool
  isInitialized() const { return __isInitialized; }

  static GLuint loadImage(const QImage&);
  static GLuint loadImage(const QString&);
  static void deleteImage(GLuint);
  static QGLFormat getFormat();

#ifndef NO_DEBUG
  static unsigned texturesCount;
#endif

signals:
  void contextMenuRequested(const Pilot*);
  void contextMenuRequested(const Airport*);
  void contextMenuRequested(const Fir*);
  void flightDetailsWindowRequested(const Client*);
  void airportDetailsWindowRequested(const Airport*);
  void firDetailsWindowRequested(const Fir*);
  void flightTrackingRequested(const Pilot*);
  void flightTrackingCanceled();
  void airportLinesToggled(const Airport*);

public slots:
  void showClient(const Client*);
  void showAirport(const Airport*);
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

private:
  enum PMMatrixMode {
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

  void __produceCircle();

  /* Some inlined helpful functions */
  float __distanceFromCamera(float, float);
  void  __mapCoordinates(float, float, float&, float&);
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

  /* Zoom factor */
  int   __zoom;

  /*Actual Zoom level*/
  int   __actualzoom;
  
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

  VatsinatorApplication&  __mother;

  VatsimDataHandler& __data;

  const AirportsMap& __airports;

  FirDatabase*           __firs;
  AirportDatabase*       __airportDatabase;
  AirportDetailsWindow*  __airportDetailsWindow;
  ATCDetailsWindow*      __atcDetailsWindow;
  FirDetailsWindow*      __firDetailsWindow;
  FlightDetailsWindow*   __flightDetailsWindow;
  MetarsWindow*          __metarsWindow;
  SettingsManager*       __settings;
  WorldMap*              __myWorldMap;

  /* Modules */
  FlightTracker*         __myFlightTracker;

#ifndef NO_DEBUG
  /* For memory tracking */
  QMap< GLuint, unsigned > __imagesMemory;
#endif
};

#endif // MAPWIDGET_H
