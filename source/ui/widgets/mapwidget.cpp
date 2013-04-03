/*
    mapwidget.cpp
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

#include <QtGui>

#include "db/airportdatabase.h"
#include "db/firdatabase.h"
#include "db/worldmap.h"

#include "glutils/glextensions.h"

#include "modules/airporttracker.h"
#include "modules/flighttracker.h"

#include "storage/settingsmanager.h"

#include "ui/userinterface.h"

#include "ui/actions/actionmenuseparator.h"
#include "ui/actions/airportdetailsaction.h"
#include "ui/actions/clientdetailsaction.h"
#include "ui/actions/firdetailsaction.h"
#include "ui/actions/metaraction.h"
#include "ui/actions/toggleinboundoutboundlinesaction.h"
#include "ui/actions/trackaction.h"

#include "ui/pages/colorspage.h"
#include "ui/pages/miscellaneouspage.h"
#include "ui/pages/viewpage.h"

#include "ui/windows/airportdetailswindow.h"
#include "ui/windows/atcdetailswindow.h"
#include "ui/windows/firdetailswindow.h"
#include "ui/windows/flightdetailswindow.h"
#include "ui/windows/metarswindow.h"

#include "vatsimdata/uir.h"
#include "vatsimdata/vatsimdatahandler.h"

#include "vatsimdata/airport/activeairport.h"
#include "vatsimdata/airport/emptyairport.h"

#include "vatsimdata/client/controller.h"
#include "vatsimdata/client/pilot.h"

#include "vatsimdata/models/controllertablemodel.h"
#include "vatsimdata/models/flighttablemodel.h"

#include "debugging/glerrors.h"

#include "vatsinatorapplication.h"

#include "mapwidget.h"
#include "defines.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

const double PI = 3.1415926535897;

const GLfloat VERTICES[] = {
  -0.04, -0.02,
  -0.04,  0.06,
   0.04,  0.06,
   0.04, -0.02
};
const GLfloat PILOT_TOOLTIP_VERTICES[] = {
  -0.16,  0.019,
  -0.16,  0.12566666,
   0.16,  0.12566666,
   0.16,  0.019
};
const GLfloat AIRPORT_TOOLTIP_VERTICES[] = {
  -0.08, -0.05333333,
  -0.08, 0,
   0.08, 0,
   0.08, -0.05333333
};
const GLfloat FIR_TOOLTIP_VERTICES[] = {
  -0.08, -0.05333333,
  -0.08,  0.05333333,
   0.08,  0.05333333,
   0.08, -0.05333333
};
const GLfloat MODEL_VERTICES[] = {
  -0.03, -0.03,
  -0.03,  0.03,
   0.03,  0.03,
   0.03, -0.03
};
const GLfloat TEXCOORDS[] = {
  0.0, 0.0,
  0.0, 1.0,
  1.0, 1.0,
  1.0, 0.0
};


#ifndef NO_DEBUG
unsigned MapWidget::texturesCount = 0;
#endif


/* Just one helpful function */

template< typename T >
inline T absHelper(const T& _v) {
  return _v < 0 ? -_v : _v;
}


QGLFormat myformat = MapWidget::getFormat();

  
MapWidget::MapWidget(QWidget* _parent) :
    QGLWidget(myformat, _parent),
    __isInitialized(false),
    __pilotToolTip(":/pixmaps/pilot_tooltip.png"),
    __pilotFont("Verdana"),
    __airportToolTip(":/pixmaps/airport_tooltip.png"),
    __airportFont("Verdana"),
    __firToolTip(64, 32, QImage::Format_ARGB32_Premultiplied),
    __firFont("Verdana"),
    __position(0.0, 0.0),
    __zoom(ZOOM_MINIMUM),
    __actualZoom(ACTUAL_ZOOM_MINIMUM),
    __actualZoomMaximum(),
    __keyPressed(false),
    __underMouse(NULL),
    __contextMenuOpened(false),
    __label(NULL),
    __menu(NULL),
    __drawLeft(false),
    __drawRight(false),
    __data(VatsimDataHandler::getSingleton()),
    __airports(__data.getActiveAirports()) {
  
  __produceCircle();

  connect(&__data, SIGNAL(vatsimDataUpdated()),
          this,    SLOT(redraw()));

  connect(this, SIGNAL(contextMenuRequested(const Pilot*)),
          this, SLOT(__openContextMenu(const Pilot*)));
  connect(this, SIGNAL(contextMenuRequested(const Airport*)),
          this, SLOT(__openContextMenu(const Airport*)));
  connect(this, SIGNAL(contextMenuRequested(const Fir*)),
          this, SLOT(__openContextMenu(const Fir*)));
  
  connect(SettingsManager::getSingletonPtr(),      SIGNAL(settingsChanged()),
          this,                                    SLOT(__loadNewSettings()));

  setAutoBufferSwap(true);

  __firFont.setPixelSize(FIR_FONT_PIXEL_SIZE);
  __firFont.setWeight(FIR_FONT_WEIGHT);

  __firToolTip.fill(0);
}

MapWidget::~MapWidget() {
  deleteImage(__apIcon);
  deleteImage(__apStaffedIcon);

  __storeSettings();
  delete [] __circle;
}

GLuint
MapWidget::loadImage(const QImage& _img) {
  GLuint pix;
  QImage final = QGLWidget::convertToGLFormat(_img);

  glEnable(GL_TEXTURE_2D); checkGLErrors(HERE);
  glGenTextures(1, &pix); checkGLErrors(HERE);

  glBindTexture(GL_TEXTURE_2D, pix); checkGLErrors(HERE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, final.width(), final.height(),
               0, GL_RGBA, GL_UNSIGNED_BYTE, final.bits()); checkGLErrors(HERE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR causes the text
  // on the tooltip is blurred

  glBindTexture(GL_TEXTURE_2D, 0);

#ifndef NO_DEBUG
  MapWidget::getSingleton().__imagesMemory[pix] = static_cast< unsigned >(final.byteCount());
  registerGPUMemoryAllocFunc(MapWidget::getSingleton().__imagesMemory[pix]);
  texturesCount += 1;
#endif

  return pix;
}

GLuint
MapWidget::loadImage(const QString& _fName) {
  GLuint pix;
  QImage final, temp;

  VatsinatorApplication::log("Loading texture: %s...", _fName.toStdString().c_str());

  if (!temp.load(_fName)) {
    VatsinatorApplication::alert("Image " + _fName + " could not be loaded!");
    VatsinatorApplication::quit();
  }

  final = QGLWidget::convertToGLFormat(temp);

  glEnable(GL_TEXTURE_2D); checkGLErrors(HERE);
  glGenTextures(1, &pix); checkGLErrors(HERE);

  glBindTexture(GL_TEXTURE_2D, pix); checkGLErrors(HERE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, final.width(), final.height(),
               0, GL_RGBA, GL_UNSIGNED_BYTE, final.bits()); checkGLErrors(HERE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);

#ifndef NO_DEBUG
  MapWidget::getSingleton().__imagesMemory[pix] = static_cast< unsigned >(final.byteCount());
  registerGPUMemoryAllocFunc(MapWidget::getSingleton().__imagesMemory[pix]);
  texturesCount += 1;
#endif

  return pix;
}

void
MapWidget::deleteImage(GLuint _tex) {
  glDeleteTextures(1, &_tex); checkGLErrors(HERE);

#ifndef NO_DEBUG
  unregisterGPUMemoryAllocFunc(MapWidget::getSingleton().__imagesMemory[_tex]);
  texturesCount -= 1;
#endif
}

QGLFormat
MapWidget::getFormat() {
  QGLFormat glf = QGLFormat::defaultFormat();
  glf.setSampleBuffers(true);
  glf.setSamples(4);
  return glf;
}

void
MapWidget::showClient(const Client* _c) {
  if (FlightTracker::getSingleton().getTracked() != _c)
    emit flightTrackingCanceled();

  __position.rx() = _c->getPosition().longitude / 180;
  __position.ry() = _c->getPosition().latitude / 90;
  updateGL();
}

void
MapWidget::showAirport(const Airport* _ap) {
  if (FlightTracker::getSingleton().getTracked())
    emit flightTrackingCanceled();
  
  Q_ASSERT(_ap->getData());

  __position.rx() = _ap->getData()->longitude / 180;
  __position.ry() = _ap->getData()->latitude / 90;
  updateGL();
}

void
MapWidget::redraw() {
  __underMouse = NULL;
  QToolTip::hideText();

  if (cursor().shape() != Qt::SizeAllCursor)
    setCursor(QCursor(Qt::ArrowCursor));

  if (__menu) {
    __menu->close();
    delete __menu;
    __menu = NULL;
  }

  updateGL();
}

void
MapWidget::initializeGL() {
  VatsinatorApplication::log("Initializing OpenGL...");
  
  makeCurrent();
  
  glEnable(GL_MULTISAMPLE);
  
#ifndef NO_DEBUG
  GLint bufs;
  GLint samples;
  glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
  glGetIntegerv(GL_SAMPLES, &samples);
  VatsinatorApplication::log("Have %d buffers and %d samples.", bufs, samples);
#endif
  
  glShadeModel(GL_SMOOTH);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  
  glEnable(GL_LINE_STIPPLE);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0.1f); checkGLErrors(HERE);
  
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glEnable(GL_DEPTH_TEST); checkGLErrors(HERE);
  glEnable(GL_TEXTURE_2D); checkGLErrors(HERE);
  glEnableClientState(GL_VERTEX_ARRAY);
  
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  
#ifndef Q_OS_DARWIN
  initGLExtensionsPointers();
#endif
  
  VatsinatorApplication::log("OpenGL set up.");
  
  QCoreApplication::flush();
  __init();
  
  __isInitialized = true;
  
  VatsinatorApplication::log("Ready to render.");
}

void
MapWidget::paintGL() {
  __underMouse = NULL;

  __drawLeft = (-1 - __position.x()) * __zoom > -__orthoRangeX;
  __drawRight = (1 - __position.x()) * __zoom < __orthoRangeX;
  __360degreesMapped = -__position.x() * __zoom;

  qglColor(__settings.colors.lands);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  qglClearColor(__settings.colors.seas);
  glLoadIdentity();

  if (FlightTracker::getSingleton().getTracked()) {
    __position.rx() = FlightTracker::getSingleton().getTracked()->getPosition().longitude / 180;
    __position.ry() = FlightTracker::getSingleton().getTracked()->getPosition().latitude / 90;
  }

  glEnableClientState(GL_VERTEX_ARRAY);
  checkGLErrors(HERE);
  __prepareMatrix(WORLD);

#ifndef NO_DEBUG
  __drawMarks();
#endif

  __drawWorld();

  if (__drawLeft)
    __drawWorld(-360.0);

  if (__drawRight)
    __drawWorld(360.0);


  __drawFirs();

  if (__drawLeft)
    __drawFirs(-360.0);

  if (__drawRight)
    __drawFirs(360.0);

  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, TEXCOORDS);

  __prepareMatrix(AIRPORTS_PILOTS);

  __drawFirsLabels();

  if (__drawLeft)
    __drawFirsLabels(-360.0);

  if (__drawRight)
    __drawFirsLabels(360.0);

  if (__settings.view.airports_layer) {
    __drawAirports();

    if (__drawLeft)
      __drawAirports(-360.0);

    if (__drawRight)
      __drawAirports(360.0);
  }

  if (__settings.view.pilots_layer) {
    __drawPilots();

    if (__drawLeft)
      __drawPilots(-360.0);

    if (__drawRight)
      __drawPilots(360.0);
  }

  __drawLines();

  if (__drawLeft)
    __drawLines(-360.0);

  if (__drawRight)
    __drawLines(360.0);

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);

  if (!__underMouse || __contextMenuOpened) {
    QToolTip::hideText();

    if (cursor().shape() != Qt::SizeAllCursor)
      setCursor(QCursor(Qt::ArrowCursor));
  } else
    __drawToolTip();
}

void
MapWidget::resizeGL(int width, int height) {
  glViewport(0, 0, width, height);

  __orthoRangeX = static_cast< GLdouble >(width) / BASE_SIZE_WIDTH;
  __orthoRangeY = static_cast< GLdouble >(height) / BASE_SIZE_HEIGHT;

  __prepareMatrix(WORLD);

  __winWidth = width;
  __winHeight = height;
  
  emit resized();
}

void
MapWidget::wheelEvent(QWheelEvent* _event) {
  int steps = _event->delta() / 120;
  __updateZoom(steps);
  
  _event->accept();

  updateGL();
}

void
MapWidget::mousePressEvent(QMouseEvent* _event) {
  __lastMousePos = _event->pos();
  QToolTip::hideText();

  if ((_event->buttons() & Qt::RightButton) && __underMouse) {
    switch (__underMouse->objectType()) {
      case Clickable::PLANE:
        emit contextMenuRequested(static_cast< const Pilot* >(__underMouse));
        break;
      case Clickable::AIRPORT:
        emit contextMenuRequested(static_cast< const Airport* >(__underMouse));
        break;
      case Clickable::FIR:
        emit contextMenuRequested(static_cast< const Fir* >(__underMouse));
        break;
      case Clickable::UIR:
        break;
    }

    __underMouse = NULL;
  } else if ((_event->buttons() & Qt::LeftButton) && __underMouse) {
    // store the clicked position - if user clicks & moves, it is just map move
    __recentlyClickedMousePos = _event->pos();
  }
}

void
MapWidget::mouseReleaseEvent(QMouseEvent* _event) {
  setCursor(QCursor(Qt::ArrowCursor));
  __lastMousePos = _event->pos();

  if (__underMouse && !__contextMenuOpened) {
    QToolTip::hideText();

    if (__recentlyClickedMousePos == __lastMousePos) {
      switch (__underMouse->objectType()) {
        case Clickable::PLANE:
          emit flightDetailsWindowRequested(static_cast< const Pilot* >(__underMouse));
          break;
        case Clickable::AIRPORT:
	  emit airportDetailsWindowRequested(static_cast< const Airport* >(__underMouse));
          break;
        case Clickable::FIR:
          emit firDetailsWindowRequested(static_cast< const Fir* >(__underMouse));
          break;
        case Clickable::UIR:
          // have no idea what to do here
          break;
      }
    }
  }

  __underMouse = NULL;
}

void
MapWidget::mouseMoveEvent(QMouseEvent* _event) {
  int dx = _event->x() - __lastMousePos.x();
  int dy = _event->y() - __lastMousePos.y();

  if (_event->buttons() & Qt::LeftButton) {
    setCursor(QCursor(Qt::SizeAllCursor));

    // count the new position
    __position.rx() -= (double)dx / (BASE_SIZE_WIDTH / 2) / (double)__zoom;

    if (__position.x() < -1)
      __position.rx() += 2;
    else if (__position.x() > 1)
      __position.rx() -= 2;

    double newY = __position.y() + ((double)dy / 300.0 / (double)__zoom);

    if ((newY < RANGE_Y) && (newY > -RANGE_Y))
      __position.setY(newY);

    if (FlightTracker::getSingleton().getTracked() && (dx != 0 || dy != 0))
      emit flightTrackingCanceled();
  }

  __lastMousePos = _event->pos();

  // counts the mouse position point, interpolated to the GL's context size
  __lastMousePosInterpolated.rx() = ((double)__lastMousePos.x() -
                                     (((double)__winWidth - BASE_SIZE_WIDTH) / 2)) / (BASE_SIZE_WIDTH / 2) - 1.0;
  __lastMousePosInterpolated.ry() = -(((double)__lastMousePos.y() -
                                       (((double)__winHeight - BASE_SIZE_HEIGHT) / 2)) / (BASE_SIZE_HEIGHT / 2) - 1.0);

  // count the mouse position global coordinates
  double longitude = (__lastMousePosInterpolated.x() / (double)__zoom + __position.x()) * 180;
  double latitude = (__lastMousePosInterpolated.y() / (double)__zoom + __position.y()) * 90;

  if (latitude < -90)
    latitude = -90;
  else if (latitude > 90)
    latitude = 90;

  if (longitude < -180)
    longitude += 360;
  else if (longitude > 180)
    longitude -= 360;

  // update the label on the very bottom of the main window
  UserInterface::getSingleton().getPositionBox()->setText(
    QString((latitude > 0) ? "N" : "S") + " " +
    QString::number(absHelper(latitude), 'g', 6) + " " +
    QString((longitude < 0) ? "W" : "E") + " " +
    QString::number(absHelper(longitude), 'g', 6) + " "
  );

  __contextMenuOpened = false;

  updateGL();
}

void
MapWidget::keyPressEvent(QKeyEvent* _event) {
  switch (_event->key()) {
    case Qt::Key_PageUp:
      __updateZoom(1);
      break;
    case Qt::Key_PageDown:
      __updateZoom(-1);
      break;
    case Qt::Key_Shift:
      __keyPressed = true;
  }

  updateGL();
}

void
MapWidget::keyReleaseEvent(QKeyEvent* _event) {
  switch (_event->key()) {
    case Qt::Key_Shift:
      __keyPressed = false;
      break;
  }

  updateGL();
}

void
MapWidget::__loadNewSettings() {
  __settings.misc.zoom_coefficient = SM::get("misc.zoom_coefficient").toBool();
  
  __settings.colors.lands = SM::get("colors.lands").value<QColor>();
  __settings.colors.seas = SM::get("colors.seas").value<QColor>();
  __settings.colors.staffed_fir_borders = SM::get("colors.staffed_fir_borders").value<QColor>();
  __settings.colors.staffed_fir_background = SM::get("colors.staffed_fir_background").value<QColor>();
  __settings.colors.staffed_uir_borders = SM::get("colors.staffed_uir_borders").value<QColor>();
  __settings.colors.staffed_uir_background = SM::get("colors.staffed_uir_background").value<QColor>();
  __settings.colors.unstaffed_fir_borders = SM::get("colors.unstaffed_fir_borders").value<QColor>();
  __settings.colors.approach_circle = SM::get("colors.approach_circle").value<QColor>();
  
  __settings.view.airports_layer = SM::get("view.airports_layer").toBool();
  __settings.view.airport_labels = SM::get("view.airport_labels").toBool();
  __settings.view.pilots_layer = SM::get("view.pilots_layer").toBool();
  __settings.view.staffed_firs = SM::get("view.staffed_firs").toBool();
  __settings.view.unstaffed_firs = SM::get("view.unstaffed_firs").toBool();
  __settings.view.empty_airports = SM::get("view.empty_airports").toBool();
  __settings.view.pilot_labels.always = SM::get("view.pilot_labels.always").toBool();
  __settings.view.pilot_labels.airport_related = SM::get("view.pilot_labels.airport_related").toBool();
  __settings.view.pilot_labels.when_hovered = SM::get("view.pilot_labels.when_hovered").toBool();
  
  __setAntyaliasing(SM::get("misc.has_antyaliasing").toBool());
}

void
MapWidget::__openContextMenu(const Pilot* _pilot) {
  __contextMenuOpened = true;
  
  __menu = new QMenu(_pilot->getCallsign(), this);

  ClientDetailsAction* showDetails = new ClientDetailsAction(_pilot, tr("Flight details"), this);
  TrackAction* trackThisFlight = new TrackAction(_pilot, this);
  __menu->addAction(showDetails);
  __menu->addAction(trackThisFlight);

  connect(showDetails,                             SIGNAL(triggered(const Client*)),
          FlightDetailsWindow::getSingletonPtr(),  SLOT(show(const Client*)));
  connect(trackThisFlight,                         SIGNAL(triggered(const Pilot*)),
          this,                                    SIGNAL(flightTrackingRequested(const Pilot*)));

  __menu->addSeparator();

  if (!_pilot->getRoute().origin.isEmpty()) {
    MetarAction* showDepMetar = new MetarAction(_pilot->getRoute().origin, this);
    __menu->addAction(showDepMetar);
    connect(showDepMetar,                    SIGNAL(triggered(QString)),
            MetarsWindow::getSingletonPtr(), SLOT(show(QString)));
  }

  if (!_pilot->getRoute().destination.isEmpty()) {
    MetarAction* showArrMetar = new MetarAction(_pilot->getRoute().destination, this);
    __menu->addAction(showArrMetar);
    connect(showArrMetar,                    SIGNAL(triggered(QString)),
            MetarsWindow::getSingletonPtr(), SLOT(show(QString)));
  }

  __menu->exec(mapToGlobal(__lastMousePos));
  delete __menu;
  __menu = NULL;
}

void
MapWidget::__openContextMenu(const Airport* _ap) {
  __contextMenuOpened = true;
  
  __menu = new QMenu(_ap->getData()->icao, this);

  AirportDetailsAction* showAp = new AirportDetailsAction(_ap, tr("Airport details"), this);
  MetarAction* showMetar = new MetarAction(_ap->getData()->icao, this);
  ToggleInboundOutboundLinesAction* toggleAction = new ToggleInboundOutboundLinesAction(_ap, this);

  __menu->addAction(showAp);
  __menu->addAction(showMetar);
  __menu->addAction(toggleAction);

  connect(showAp,                                    SIGNAL(triggered(const Airport*)),
          AirportDetailsWindow::getSingletonPtr(),   SLOT(show(const Airport*)));

  connect(showMetar,                                 SIGNAL(triggered(QString)),
          MetarsWindow::getSingletonPtr(),           SLOT(show(QString)));
  
  connect(toggleAction,                              SIGNAL(triggered(const Airport*)),
          this,                                      SIGNAL(airportLinesToggled(const Airport*)));

  if (dynamic_cast< const ActiveAirport* >(_ap) != NULL) {
    const ActiveAirport* aa = dynamic_cast< const ActiveAirport* >(_ap);
    if (!aa->getStaffModel()->getStaff().isEmpty()) {
      __menu->addSeparator();
      __menu->addAction(new ActionMenuSeparator(tr("Controllers"), this));
  
      for (const Controller* c: aa->getStaffModel()->getStaff()) {
        ClientDetailsAction* showDetails = new ClientDetailsAction(c, c->getCallsign(), this);
        __menu->addAction(showDetails);
        connect(showDetails,                         SIGNAL(triggered(const Client*)),
                AtcDetailsWindow::getSingletonPtr(), SLOT(show(const Client*)));
      }
    }
  
    if (!aa->getOutboundsModel()->getFlights().isEmpty() && aa->countDepartures()) {
      __menu->addSeparator();
      __menu->addAction(new ActionMenuSeparator(tr("Departures"), this));
      
      for (const Pilot* p: aa->getOutboundsModel()->getFlights()) {
        if (p->getFlightStatus() != Pilot::DEPARTING)
          continue;
      
        ClientDetailsAction* showDetails = new ClientDetailsAction(
            p,
            /* For example: BAW123 to EGLL */
            tr("%1 to %2").arg(
              p->getCallsign(),
              /* "Nowhere" means there's no destination airport in flight plan yet. */
              (p->getRoute().destination.isEmpty() ? tr("nowhere") : p->getRoute().destination)
            ),
            this
          );
        __menu->addAction(showDetails);
        
        if (p->isPrefiledOnly())
          showDetails->setEnabled(false);
        else
          connect(showDetails,                             SIGNAL(triggered(const Client*)),
                  FlightDetailsWindow::getSingletonPtr(),  SLOT(show(const Client*)));
      }
    }
  
    if (!aa->getInboundsModel()->getFlights().isEmpty() && aa->countArrivals()) {
      __menu->addSeparator();
      __menu->addAction(new ActionMenuSeparator(tr("Arrivals"), this));
    
      for (const Pilot* p: aa->getInboundsModel()->getFlights()) {
        if (p->getFlightStatus() != Pilot::ARRIVED)
          continue;
      
        ClientDetailsAction* showDetails = new ClientDetailsAction(
            p,
            /* For example: BAW123 from EGLL */
            tr("%1 from %2").arg(
              p->getCallsign(),
              p->getRoute().origin
            ),
            this
          );
        __menu->addAction(showDetails);
        
        if (p->isPrefiledOnly())
          showDetails->setEnabled(false);
        else
          connect(showDetails,                             SIGNAL(triggered(const Client*)),
                  FlightDetailsWindow::getSingletonPtr(),  SLOT(show(const Client*)));
      }
    }
  }

  __menu->exec(mapToGlobal(__lastMousePos));
  delete __menu;
  __menu = NULL;
}

void
MapWidget::__openContextMenu(const Fir* _fir) {
  __contextMenuOpened = true;
  
  __menu = new QMenu(_fir->getIcao(), this);

  FirDetailsAction* showFir = new FirDetailsAction(_fir,
      /* FIR details */
      tr("%1 details").arg(_fir->getIcao()), this);

  __menu->addAction(showFir);

  connect(showFir,                             SIGNAL(triggered(const Fir*)),
          FirDetailsWindow::getSingletonPtr(), SLOT(show(const Fir*)));

  for (const Controller* c: _fir->getStaffModel()->getStaff()) {
    ClientDetailsAction* showDetails = new ClientDetailsAction(c, c->getCallsign(), this);
    __menu->addAction(showDetails);
    connect(showDetails,                         SIGNAL(triggered(const Client*)),
            AtcDetailsWindow::getSingletonPtr(), SLOT(show(const Client*)));
  }

  __menu->exec(mapToGlobal(__lastMousePos));
  
  delete __menu;
  __menu = NULL;
}

void
MapWidget::__openContextMenu() {
  __contextMenuOpened = true;
  
  __menu = new QMenu(tr("This location"), this);
  
  QAction* setAsHome = new QAction(tr("Set as home location"), this);
  __menu->addAction(setAsHome);
  
  __menu->addSeparator();
  __menu->addAction(new ActionMenuSeparator(tr("Flights nearby"), this));
  
  __menu->exec(mapToGlobal(__lastMousePos));
  
  delete __menu;
  __menu = NULL;
}

void
MapWidget::__init() {
  setEnabled(true);

  VatsinatorApplication::log("Loading images...");

  __apIcon = loadImage(":/pixmaps/airport.png");
  __apStaffedIcon = loadImage(":/pixmaps/airport_staffed.png");
  __apInactiveIcon = loadImage(":/pixmaps/airport_inactive.png");

  VatsinatorApplication::log("Preparing signals & slots...");

  connect(this,                                    SIGNAL(firDetailsWindowRequested(const Fir*)),
          FirDetailsWindow::getSingletonPtr(),     SLOT(show(const Fir*)));
  connect(this,                                    SIGNAL(flightDetailsWindowRequested(const Client*)),
          FlightDetailsWindow::getSingletonPtr(),  SLOT(show(const Client*)));
  connect(this,                                    SIGNAL(airportDetailsWindowRequested(const Airport*)),
          AirportDetailsWindow::getSingletonPtr(), SLOT(show(const Airport*)));
  connect(SettingsManager::getSingletonPtr(),      SIGNAL(settingsChanged()),
          this,                                    SLOT(redraw()));

  VatsinatorApplication::log("Setting fonts...");
  __pilotFont.setPixelSize(PILOT_FONT_PIXEL_SIZE);
  __pilotFont.setWeight(PILOT_FONT_WEIGHT);

  __airportFont.setPixelSize(AIRPORT_FONT_PIXEL_SIZE);
  __airportFont.setWeight(AIRPORT_FONT_WEIGHT);

  VatsinatorApplication::log("Restoring settings...");
  __restoreSettings();
//   __loadNewSettings();

  VatsinatorApplication::emitGLInitialized();
}

inline void
MapWidget::__prepareMatrix(PMMatrixMode _mode, double _moveX) {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(
    -__orthoRangeX,
    __orthoRangeX,
    -__orthoRangeY,
    __orthoRangeY,
    -1.0, 1.0
  );

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  switch(_mode) {
    case WORLD:
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      glScalef(1.0f / 180.0f, 1.0f / 90.0f, 1.0f);
      glScalef(__zoom, __zoom, 1.0);
      glTranslated(-__position.x() * 180, -__position.y() * 90, 0.0);

      if (_moveX)
        glTranslated(_moveX, 0.0, 0.0);

      checkGLErrors(HERE);
      break;
    case AIRPORTS_PILOTS:
      break;
  }
}

#ifndef NO_DEBUG
void
MapWidget::__drawMarks() {
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisable(GL_TEXTURE_2D);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glColor4f(0.0, 0.0, 0.0, 1.0);
  glPointSize(5.0);
  glBegin(GL_POINTS);
  // Lopp Lagoon, Alaska, USA
  glVertex2f(-168.010255, 65.658275);

  // Jastrzębia Góra, Poland
  glVertex2f(18.316498, 54.830754);

  // the most eastern coast of Russia :)
  glVertex2f(-169.764405, 66.10717);

  // Tierra del Fuego, Argentina
  glVertex2f(-65.166321, -54.664301);

  // Aghulas National Park, Republic of South Africa
  glVertex2f(20.000153, -34.827332);

  // Steward Island, New Zealand
  glVertex2f(167.705383, -47.118738);

  glEnd();
  checkGLErrors(HERE);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnable(GL_TEXTURE_2D);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, 0, TEXCOORDS); checkGLErrors(HERE);
}
#endif


inline void
MapWidget::__drawWorld(double _moveX) {
  glPushMatrix();
  glTranslatef(_moveX, 0.0, -0.9);

  qglColor(__settings.colors.lands);
  WorldMap::getSingleton().draw();
  checkGLErrors(HERE);
  glPopMatrix();
}

void
MapWidget::__drawFirs(double _moveX) {
  if (__settings.view.unstaffed_firs) {
    glPushMatrix();
    glTranslatef(_moveX, 0.0, -0.8);

    for (const Fir& fir: FirDatabase::getSingleton().getFirs()) {
      if (fir.isStaffed()) {
        continue;
      }

      qglColor(__settings.colors.unstaffed_fir_borders);
      fir.drawBorders(); checkGLErrors(HERE);
    }

    glPopMatrix();
  }

  if (__settings.view.staffed_firs) {
    __drawUirs(_moveX);

    glLineWidth(3.0);
    glPushMatrix();
    glTranslatef(_moveX, 0.0, -0.6);

    for (const Fir& fir: FirDatabase::getSingleton().getFirs()) {
      if (!fir.isStaffed())
        continue;

      qglColor(__settings.colors.staffed_fir_borders);
      fir.drawBorders(); checkGLErrors(HERE);

      qglColor(__settings.colors.staffed_fir_background);
      fir.drawTriangles(); checkGLErrors(HERE);
    }

    glPopMatrix();
    glLineWidth(1.0);
  }
}

inline void
MapWidget::__drawUirs(double _moveX) {
  glLineWidth(3.0);

  glPushMatrix();
  glTranslatef(_moveX, 0.0, -0.7);

  for (const Uir * uir: __data.getUIRs()) {
    if (!uir->isEmpty()) {
      for (const Fir * fir: uir->getRange()) {
        if (!fir->isStaffed()) {
          qglColor(__settings.colors.staffed_uir_borders);
          fir->drawBorders(); checkGLErrors(HERE);

          qglColor(__settings.colors.staffed_uir_background);
          fir->drawTriangles(); checkGLErrors(HERE);
        }
      }
    }

    checkGLErrors(HERE);
  }

  glPopMatrix();

  qglColor(__settings.colors.seas);
  glLineWidth(1.0);
  checkGLErrors(HERE);
}

void
MapWidget::__drawFirsLabels(float _moveX) {
  glColor4f(1.0, 1.0, 1.0, 1.0);
  glPushMatrix();
  glTranslatef(0.0, 0.0, -0.5);

  for (const Fir& fir: FirDatabase::getSingleton().getFirs()) {
    if (fir.getTextPosition().x == 0.0 && fir.getTextPosition().y == 0.0)
      continue;

    float x, y;
    __mapCoordinates(fir.getTextPosition().x + _moveX, fir.getTextPosition().y, x, y);

    if ((x <= __orthoRangeX) && (y <= __orthoRangeY) &&
        (x >= -__orthoRangeX) && (y >= -__orthoRangeY)) {

      __drawFirLabel(x, y, fir);
      checkGLErrors(HERE);

      if (__distanceFromCamera(x, y) < OBJECT_TO_MOUSE &&
          !__underMouse) {
        __underMouse = &fir;
      }
    }
  }

  glPopMatrix();
  checkGLErrors(HERE);
}

void
MapWidget::__drawAirports(float _moveX) {
  glColor4f(1.0, 1.0, 1.0, 1.0);
  
//   Draw inactive airports 
  if (__settings.view.empty_airports || __keyPressed) {
    for (AirportRecord& ap: AirportDatabase::getSingleton().getAirports()) {
      if (__airports.contains(ap.icao))
        continue;
      
      GLfloat x = (((ap.longitude + _moveX) / 180) - __position.x()) * __zoom;
      
      if (x < -__orthoRangeX || x > __orthoRangeX)
        continue;
      
      GLfloat y = ((ap.latitude / 90) - __position.y()) * __zoom;
      
      if (y < -__orthoRangeY || y > __orthoRangeY)
        continue;
      
      bool inRange = __distanceFromCamera(x, y) < OBJECT_TO_MOUSE;
      if (inRange && !__underMouse)
        __underMouse = __data.addEmptyAirport(&ap);
      
      glVertexPointer(2, GL_FLOAT, 0, VERTICES); checkGLErrors(HERE);
      
      glBindTexture(GL_TEXTURE_2D, __apInactiveIcon); checkGLErrors(HERE);
      
      glPushMatrix();
      
        glTranslatef(x, y, -0.5); checkGLErrors(HERE);
        glDrawArrays(GL_QUADS, 0, 4); checkGLErrors(HERE);
      
      glPopMatrix();
    }
  }

//   And then draw active airports 
  for (auto it = __airports.begin(); it != __airports.end(); ++it) {

    if (!it.value()->getData())
      continue;

    GLfloat x = (((it.value()->getData()->longitude + _moveX) / 180) - __position.x()) * __zoom;

    if (x < -__orthoRangeX || x > __orthoRangeX)
      continue;

    GLfloat y = ((it.value()->getData()->latitude / 90) - __position.y()) * __zoom;

    if (y < -__orthoRangeY || y > __orthoRangeY)
      continue;

    glVertexPointer(2, GL_FLOAT, 0, VERTICES); checkGLErrors(HERE);

    bool inRange = __distanceFromCamera(x, y) < OBJECT_TO_MOUSE;

    glBindTexture(GL_TEXTURE_2D, (it.value()->getStaffModel()->getStaff().isEmpty()) ? __apIcon : __apStaffedIcon );
    checkGLErrors(HERE);

    glPushMatrix();

      glTranslatef(x, y, -0.4); checkGLErrors(HERE);
      glDrawArrays(GL_QUADS, 0, 4); checkGLErrors(HERE);
      
      if (__settings.view.airport_labels || __keyPressed)
	__drawIcaoLabel(it.value()); checkGLErrors(HERE);
      
      if (inRange && !__underMouse)
	__underMouse = it.value();
      
      if (it.value()->hasApproach()) {
	glBindTexture(GL_TEXTURE_2D, 0); checkGLErrors(HERE);
  
	qglColor(__settings.colors.approach_circle);
	glVertexPointer(2, GL_FLOAT, 0, __circle); checkGLErrors(HERE);
  
	glLineWidth(1.5);
	glLineStipple(1, 0xF0F0);
	glPushMatrix();
	glScalef(0.005f * __zoom, 0.005f * __zoom, 0); checkGLErrors(HERE);
	glDrawArrays(GL_LINE_LOOP, 0, __circleCount);
	glPopMatrix();
	glLineWidth(1.0);
	glLineStipple(1, 0xFFFF); checkGLErrors(HERE);
	
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glVertexPointer(2, GL_FLOAT, 0, VERTICES);
      }
      
    glPopMatrix();

    if (inRange && __underMouse == it.value() &&
        !__settings.view.pilot_labels.always &&
        __settings.view.pilot_labels.airport_related &&
        !__keyPressed) { // draw callsign labels if not drawn already
      float tipX, tipY;

      for (const Pilot * p: it.value()->getOutboundsModel()->getFlights()) {
        if (p->getFlightStatus() == Pilot::AIRBORNE) {
          __mapCoordinates(p->getPosition().longitude, p->getPosition().latitude,
                           tipX, tipY);
          __drawCallsign(tipX, tipY, p);
        }
      }

      for (const Pilot * p: it.value()->getInboundsModel()->getFlights()) {
        if (p->getFlightStatus() == Pilot::AIRBORNE) {
          __mapCoordinates(p->getPosition().longitude, p->getPosition().latitude,
                           tipX, tipY);
          __drawCallsign(tipX, tipY, p);
        }
      }
    }
  }

  glBindTexture(GL_TEXTURE_2D, 0); checkGLErrors(HERE);
}

void
MapWidget::__drawPilots(float _moveX) {
  glColor4f(1.0, 1.0, 1.0, 1.0);
  
  for (const Pilot * client: VatsimDataHandler::getSingleton().getFlightsModel()->getFlights()) {
    Q_CHECK_PTR(client);
    if (client->getFlightStatus() != Pilot::AIRBORNE || client->isPrefiledOnly())
      continue;

    GLfloat x = ((client->getPosition().longitude + _moveX) / 180 - __position.x()) * __zoom;

    if (x < -__orthoRangeX || x > __orthoRangeX)
      continue;

    GLfloat y = (client->getPosition().latitude / 90 - __position.y()) * __zoom;

    if (y < -__orthoRangeY || y > __orthoRangeY)
      continue;

    glPushMatrix();
    bool inRange = __distanceFromCamera(x, y) < OBJECT_TO_MOUSE;

    glColor4f(1.0, 1.0, 1.0, 1.0);

    glTranslatef(x, y, -0.2); checkGLErrors(HERE);

    glPushMatrix();
    glRotatef(static_cast< GLfloat >(client->getHeading()), 0, 0, -1); checkGLErrors(HERE);

    glVertexPointer(2, GL_FLOAT, 0, MODEL_VERTICES); checkGLErrors(HERE);
    glBindTexture(GL_TEXTURE_2D, client->getModelTexture()); checkGLErrors(HERE);

    glDrawArrays(GL_QUADS, 0, 4); checkGLErrors(HERE);
    glPopMatrix();

    if (inRange && !__underMouse)
      __underMouse = client;

    if (((__settings.view.pilot_labels.when_hovered)
         && (__keyPressed || inRange))
        || (__settings.view.pilot_labels.always))
      __drawCallsign(client);

    glPopMatrix(); checkGLErrors(HERE);
  }

  glBindTexture(GL_TEXTURE_2D, 0); checkGLErrors(HERE);
}

void
MapWidget::__drawLines(double _moveX) {
  __prepareMatrix(WORLD, _moveX);
  
  if (__keyPressed) {
    for (const Pilot* p: VatsimDataHandler::getSingleton().getFlightsModel()->getFlights()) {
      if (p->getFlightStatus() == Pilot::AIRBORNE)
        p->drawLines();
    }
  
    return;
  }
  
  if (__underMouse) {
    switch (__underMouse->objectType()) {
      case Clickable::PLANE:
        static_cast< const Pilot* >(__underMouse)->drawLines();
        break;
      case Clickable::AIRPORT:
        static_cast< const Airport* >(__underMouse)->drawLines();
        break;
      default:
        break;
    }
  }
  
  if (FlightTracker::getSingleton().getTracked())
    FlightTracker::getSingleton().getTracked()->drawLines();
  
  if (AirportTracker::getSingleton().isInitialized()) {
    for (auto it: AirportTracker::getSingleton().getTracked().values()) {
      Q_CHECK_PTR(it);
      it->drawLines();
    }
  }
}

void
MapWidget::__drawToolTip() {
  setCursor(QCursor(Qt::PointingHandCursor));
  
  QString text;

  switch (__underMouse->objectType()) {
    case Clickable::PLANE:
      text = __producePilotToolTip(static_cast< const Pilot* >(__underMouse));
      break;
    case Clickable::AIRPORT:
      text = __produceAirportToolTip(static_cast< const Airport* >(__underMouse));
      break;
    case Clickable::FIR:
      text = __produceFirToolTip(static_cast< const Fir* >(__underMouse));
      break;
    case Clickable::UIR:
      break;
  }

  QToolTip::showText(mapToGlobal(__lastMousePos), text, this);
}

void
MapWidget::__setAntyaliasing(bool _on) {
  if (_on) {
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST); checkGLErrors(HERE);
    glEnable(GL_LINE_SMOOTH); checkGLErrors(HERE);
  } else {
    glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST); checkGLErrors(HERE);
    glDisable(GL_LINE_SMOOTH); checkGLErrors(HERE);
  }
}

void
MapWidget::__storeSettings() {
  QSettings settings;

  settings.beginGroup("CameraSettings");

  settings.setValue("zoomFactor", __zoom);
  settings.setValue("actualZoomCoefficient", __actualZoom);
  settings.setValue("cameraPosition", __position);

  settings.endGroup();
}

void
MapWidget::__restoreSettings() {
  QSettings settings;

  settings.beginGroup("CameraSettings");

  __zoom = settings.value("zoomFactor", ZOOM_MINIMUM).toFloat();
  __actualZoom = settings.value("actualZoomCoefficient", ACTUAL_ZOOM_MINIMUM).toInt();
  __position = settings.value("cameraPosition", QPointF(0.0, 0.0)).toPointF();

  settings.endGroup();
}

void
MapWidget::__updateZoom(int _steps) {
  //count limiter for this function
  __actualZoomMaximum =
      qFloor(qLn((ZOOM_MAXIMUM - ZOOM_MINIMUM) / ZOOM_NORMALIZE_COEFFICIENT) /
      qLn(ZOOM_BASE + (__settings.misc.zoom_coefficient * 0.01)));
  
  //set the actual zoom level according to number of scroll wheel steps
  __actualZoom += _steps;
  
  //limiting range of zoom
  __actualZoom = qBound(0, __actualZoom, __actualZoomMaximum);
  
  // count value of closeup
  __zoom = ZOOM_MINIMUM + ZOOM_NORMALIZE_COEFFICIENT *
      qPow(ZOOM_BASE + (__settings.misc.zoom_coefficient * 0.01),
           (__actualZoom));
}

void
MapWidget::__produceCircle() {
  __circleCount = 0;

  // count how many vertices we will have
  for (double angle = 0.0; angle <= (2 * PI); angle += 0.1, ++__circleCount);

  __circle = new GLfloat[__circleCount * 2 + 2];
  unsigned i = 0;

  float x, y;

  for (qreal angle = 0.0; angle <= (2 * PI); angle += 0.1) {
    x = qCos(angle);
    y = qSin(angle);
    __circle[i++] = x;
    __circle[i++] = y;
  }
}

inline float
MapWidget::__distanceFromCamera(float _x, float _y) {
  return sqrt(
           pow(_x - __lastMousePosInterpolated.x(), 2) +
           pow(_y - __lastMousePosInterpolated.y(), 2)
         );
}

inline void
MapWidget::__mapCoordinates(float _xFrom, float _yFrom,
                            float& _xTo, float& _yTo) {
  _xTo = (_xFrom / 180 - __position.x()) * __zoom;
  _yTo = (_yFrom / 90 - __position.y()) * __zoom;
}

inline QString
MapWidget::__producePilotToolTip(const Pilot* _p) {
  return
    static_cast< QString >("<center>") %
    _p->getCallsign() % "<br><nobr>" %
    _p->getRealName() % " (" % _p->getAircraft() % ")</nobr><br><nobr>" %
    (_p->getRoute().origin.isEmpty() ? tr("(unknown)") : (__airports[_p->getRoute().origin]->getData() ?
        _p->getRoute().origin % " " % QString::fromUtf8(__airports[_p->getRoute().origin]->getData()->city) :
        _p->getRoute().origin)) %
    " > " %
    (_p->getRoute().destination.isEmpty() ? tr("(unknown)") : (__airports[_p->getRoute().destination]->getData() ?
        _p->getRoute().destination % " " % QString::fromUtf8(__airports[_p->getRoute().destination]->getData()->city) :
        _p->getRoute().destination)) %
    "</nobr><br>" %
    tr("Ground speed: %1 kts").arg(QString::number(_p->getGroundSpeed())) %
    "<br>" %
    tr("Altitude: %1 ft").arg(QString::number(_p->getAltitude())) %
    "</center>";
}

inline QString
MapWidget::__produceAirportToolTip(const Airport* _ap) {
  QString text = static_cast< QString >("<center>") %
                 static_cast< QString >(_ap->getData()->icao) %
                 static_cast< QString >("<br><nobr>") %
                 QString::fromUtf8(_ap->getData()->name) %
                 static_cast< QString >(", ") %
                 QString::fromUtf8(_ap->getData()->city) %
                 static_cast< QString >("</nobr>");
  if (dynamic_cast< const ActiveAirport* >(_ap) != NULL) {
    const ActiveAirport* aa = dynamic_cast< const ActiveAirport* >(_ap);
    for (const Controller * c: aa->getStaffModel()->getStaff())
      text.append((QString)"<br><nobr>" %
                  c->getCallsign() % " " % c->getFrequency() % " " % c->getRealName() %
                  "</nobr>"
                 );
  }
  
  int deps = _ap->countDepartures();

  if (deps)
    text.append(static_cast< QString >("<br>") % tr("Departures: %1").arg(QString::number(deps)));

  int arrs = _ap->countArrivals();

  if (arrs)
    text.append(static_cast< QString >("<br>") % tr("Arrivals: %1").arg(QString::number(arrs)));

  text.append("</center>");
  return text;
}

inline QString
MapWidget::__produceFirToolTip(const Fir* _f) {
  if (_f->getName().isEmpty() && !_f->isStaffed())
    return "";

  QString text = "<center>";

  if (!_f->getName().isEmpty()) {
    text.append(static_cast< QString >("<nobr>") % _f->getName());

    if (!_f->getCountry().isEmpty())
      text.append(static_cast< QString >(", ") % _f->getCountry());

    text.append(static_cast< QString >("</nobr>"));
  }

  for (const Controller * c: _f->getStaffModel()->getStaff())
    text.append(static_cast< QString >("<br><nobr>") %
                c->getCallsign() % " " % c->getFrequency() % " " % c->getRealName() %
                static_cast< QString >("</nobr>")
               );

  text.append("</center>");
  return text;
}

inline void
MapWidget::__drawCallsign(const Pilot* _p) {
  glPushMatrix();

  if (__underMouse == _p)
    glTranslatef(0.0f, 0.0f, 0.2f);
  else
    glTranslatef(0.0f, 0.0f, 0.1f);

  glBindTexture(GL_TEXTURE_2D, _p->getCallsignTip()); checkGLErrors(HERE);
  glVertexPointer(2, GL_FLOAT, 0, PILOT_TOOLTIP_VERTICES); checkGLErrors(HERE);
  glDrawArrays(GL_QUADS, 0, 4); checkGLErrors(HERE);
  glBindTexture(GL_TEXTURE_2D, 0);
  glPopMatrix(); checkGLErrors(HERE);
}

inline void
MapWidget::__drawCallsign(GLfloat _x, GLfloat _y, const Pilot* _p) {
  glPushMatrix();
  glTranslatef(_x, _y, 0.1f);
  glBindTexture(GL_TEXTURE_2D, _p->getCallsignTip()); checkGLErrors(HERE);
  glVertexPointer(2, GL_FLOAT, 0, PILOT_TOOLTIP_VERTICES); checkGLErrors(HERE);
  glDrawArrays(GL_QUADS, 0, 4); checkGLErrors(HERE);
  glBindTexture(GL_TEXTURE_2D, 0);
  glPopMatrix(); checkGLErrors(HERE);
}

inline void
MapWidget::__drawIcaoLabel(const Airport* _ap) {
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 0.1f);
  glBindTexture(GL_TEXTURE_2D, _ap->getLabelTip()); checkGLErrors(HERE);
  glVertexPointer(2, GL_FLOAT, 0, AIRPORT_TOOLTIP_VERTICES); checkGLErrors(HERE);
  glDrawArrays(GL_QUADS, 0, 4); checkGLErrors(HERE);
  glBindTexture(GL_TEXTURE_2D, 0);
  glPopMatrix(); checkGLErrors(HERE);
}

inline void
MapWidget::__drawFirLabel(GLfloat _x, GLfloat _y, const Fir& _f) {
  if (_f.getIcaoTip()) {
    glPushMatrix();
    glTranslatef(_x, _y, 0.0f);
    glBindTexture(GL_TEXTURE_2D, _f.getIcaoTip()); checkGLErrors(HERE);
    glVertexPointer(2, GL_FLOAT, 0, FIR_TOOLTIP_VERTICES); checkGLErrors(HERE);
    glDrawArrays(GL_QUADS, 0, 4); checkGLErrors(HERE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPopMatrix(); checkGLErrors(HERE);
  }
}
