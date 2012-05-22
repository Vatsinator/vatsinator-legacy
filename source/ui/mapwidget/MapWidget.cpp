/*
    MapWidget.cpp
    Copyright (C) 2012  Michał Garapich garrappachc@gmail.com

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

#include "db/AirportsDatabase.h"
#include "db/FirsDatabase.h"
#include "db/WorldMap.h"

#include "glutils/glExtensions.h"

#include "modules/FlightTracker.h"

#include "settings/SettingsManager.h"

#include "ui/UserInterface.h"
#include "ui/actions/ActionMenuSeparator.h"
#include "ui/actions/AirportDetailsAction.h"
#include "ui/actions/ClientDetailsAction.h"
#include "ui/actions/FirDetailsAction.h"
#include "ui/actions/MetarAction.h"
#include "ui/actions/TrackAction.h"
#include "ui/windows/AirportDetailsWindow.h"
#include "ui/windows/ATCDetailsWindow.h"
#include "ui/windows/FirDetailsWindow.h"
#include "ui/windows/FlightDetailsWindow.h"
#include "ui/windows/MetarsWindow.h"

#include "vdebug/glErrors.h"

#include "VatsinatorApplication.h"

#include "MapWidget.h"

#include "defines.h"

const double PI = 3.1415926535897;

const GLfloat VERTICES[] = {
	-0.025, -0.025,
	-0.025,  0.025,
	 0.025,  0.025,
	 0.025, -0.025 };
const GLfloat PILOT_TOOLTIP_VERTICES[] = {
	-0.09375,  0.02,
	-0.09375,  0.1,
	 0.09375,  0.1,
	 0.09375,  0.02 };
const GLfloat AIRPORT_TOOLTIP_VERTICES[] = {
	-0.06, -0.065,
	-0.06, -0.025,
	 0.06, -0.025,
	 0.06, -0.065 };
const GLfloat FIR_TOOLTIP_VERTICES[] = {
	-0.08, -0.04,
	-0.08,  0.04,
	 0.08,  0.04,
	 0.08, -0.04 };
const GLfloat TEXCOORDS[] = {
	 0.0, 0.0,
	 0.0, 1.0,
	 1.0, 1.0,
	 1.0, 0.0 };

	 /* Some helpful functions */
	 
template< typename T >
inline T absHelper(const T& _v) {
	if (_v < 0)
		return -_v;
	return _v;
}

inline void drawCallsign(const Pilot* _p) {
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.1f);
		glBindTexture(GL_TEXTURE_2D, _p->callsignTip); checkGLErrors(HERE);
		glVertexPointer(2, GL_FLOAT, 0, PILOT_TOOLTIP_VERTICES); checkGLErrors(HERE);
		glDrawArrays(GL_QUADS, 0, 4); checkGLErrors(HERE);
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix(); checkGLErrors(HERE);
}

inline void drawCallsign(GLfloat _x, GLfloat _y, const Pilot* _p) {
	glPushMatrix();
		glTranslatef(_x, _y, 0.1f);
		glBindTexture(GL_TEXTURE_2D, _p->callsignTip); checkGLErrors(HERE);
		glVertexPointer(2, GL_FLOAT, 0, PILOT_TOOLTIP_VERTICES); checkGLErrors(HERE);
		glDrawArrays(GL_QUADS, 0, 4); checkGLErrors(HERE);
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix(); checkGLErrors(HERE);
}

inline void drawIcaoLabel(const AirportObject* _ap) {
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, 0.1f);
		glBindTexture(GL_TEXTURE_2D, _ap->labelTip); checkGLErrors(HERE);
		glVertexPointer(2, GL_FLOAT, 0, AIRPORT_TOOLTIP_VERTICES); checkGLErrors(HERE);
		glDrawArrays(GL_QUADS, 0, 4); checkGLErrors(HERE);
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix(); checkGLErrors(HERE);
}

inline void drawFirLabel(GLfloat _x, GLfloat _y, const Fir& _f) {
	if (_f.icaoTip) {
		glPushMatrix();
			glTranslatef(_x, _y, 0.0f);
			glBindTexture(GL_TEXTURE_2D, _f.icaoTip); checkGLErrors(HERE);
			glVertexPointer(2, GL_FLOAT, 0, FIR_TOOLTIP_VERTICES); checkGLErrors(HERE);
			glDrawArrays(GL_QUADS, 0, 4); checkGLErrors(HERE);
			glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();checkGLErrors(HERE);
	}
}


#ifndef NO_DEBUG
unsigned MapWidget::texturesCount = 0;
#endif


QGL::FormatOptions myFormat =
		QGL::DoubleBuffer
	|	QGL::DepthBuffer
	|	QGL::Rgba
	|	QGL::AlphaChannel
	|	QGL::DeprecatedFunctions
	;

MapWidget::MapWidget(QWidget* _parent) :
		QGLWidget(QGLFormat(myFormat), _parent),
		__pilotToolTip(":/pixmaps/pilot_tooltip.png"),
		__pilotFont("Verdana"),
		__airportToolTip(":/pixmaps/airport_tooltip.png"),
		__airportFont("Verdana"),
		__firToolTip(64, 24, QImage::Format_ARGB32_Premultiplied),
		__firFont("Verdana"),
		__position(0.0, 0.0),
		__zoom(ZOOM_MINIMUM),
		__keyPressed(false),
		__underMouse(NULL),
		__dontDisplayTooltip(false),
		__label(NULL),
		__mother(VatsinatorApplication::GetSingleton()),
		__data(VatsimDataHandler::GetSingleton()),
		__airports(__data.getActiveAirports()) {
	__produceCircle();
	
	connect(VatsinatorApplication::GetSingletonPtr(),	SIGNAL(dataUpdated()),
		this,						SLOT(redraw()));
	
	connect(AirportDetailsWindow::GetSingletonPtr(),	SIGNAL(showPilotRequest(const Pilot*)),
		this,						SLOT(showPilot(const Pilot*)));
	
	connect(FirDetailsWindow::GetSingletonPtr(),		SIGNAL(showAirportRequest(const AirportObject*)),
		this,						SLOT(showAirport(const AirportObject*)));
	
	connect(FirDetailsWindow::GetSingletonPtr(),		SIGNAL(showPilotRequest(const Pilot*)),
		this,						SLOT(showPilot(const Pilot*)));
	
	connect(this,	SIGNAL(contextMenuRequested(const Pilot*)),
		this,	SLOT(__openContextMenu(const Pilot*)));
	connect(this,	SIGNAL(contextMenuRequested(const AirportObject*)),
		this,	SLOT(__openContextMenu(const AirportObject*)));
	connect(this,	SIGNAL(contextMenuRequested(const Fir*)),
		this,	SLOT(__openContextMenu(const Fir*)));
	
	setAutoBufferSwap(true);
	
	__firFont.setPixelSize(FIR_FONT_PIXEL_SIZE);
	__firFont.setWeight(FIR_FONT_WEIGHT);
	
	__firToolTip.fill(0);
}

MapWidget::~MapWidget() {
	deleteImage(__apIcon);
	deleteImage(__apStaffedIcon);
	deleteImage(__pilotIcon);
	
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
	MapWidget::GetSingleton().__imagesMemory[pix] = static_cast< unsigned >(final.byteCount());
	registerGPUMemoryAllocFunc(MapWidget::GetSingleton().__imagesMemory[pix]);
	texturesCount += 1;
#endif
	
	return pix;
}

GLuint
MapWidget::loadImage(const QString& _fName) {
	GLuint pix;
	QImage final, temp;
	
	if (!temp.load(_fName)) {
		VatsinatorApplication::alert("Image could not be loaded!");
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
	MapWidget::GetSingleton().__imagesMemory[pix] = static_cast< unsigned >(final.byteCount());
	registerGPUMemoryAllocFunc(MapWidget::GetSingleton().__imagesMemory[pix]);
	texturesCount += 1;
#endif
	
	return pix;
}

void
MapWidget::deleteImage(GLuint _tex) {
	glDeleteTextures(1, &_tex); checkGLErrors(HERE);
	
#ifndef NO_DEBUG
	unregisterGPUMemoryAllocFunc(MapWidget::GetSingleton().__imagesMemory[_tex]);
	texturesCount -= 1;
#endif
}

void
MapWidget::showPilot(const Pilot* _p) {
	__position.rx() = _p->position.longitude / 180;
	__position.ry() = _p->position.latitude / 90;
	updateGL();
}

void
MapWidget::showAirport(const AirportObject* _ap) {
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
	
	updateGL();
}

void
MapWidget::initializeGL() {
#ifndef NO_DEBUG
	qDebug() << "Initializing OpenGL...";
	
#endif
	makeCurrent();
	
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
	
#ifndef NO_DEBUG
	QGLFormat::OpenGLVersionFlags ogvf = QGLFormat::openGLVersionFlags();
	qDebug() << "OpenGL version flags: " << ogvf;
#endif
	
	initGLExtensionsPointers();
	
#ifndef NO_DEBUG
	qDebug() << "OpenGL ready.";
#endif
	
	QCoreApplication::flush();
	__init();
	
#ifndef NO_DEBUG
	qDebug() << "Ready to render.";
#endif
}



void
MapWidget::paintGL() {
	__underMouse = NULL;
	
	qglColor(__settings->getSeasColor());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	qglClearColor(__settings->getSeasColor());
	glLoadIdentity();
	
	if (__myFlightTracker->getTracked()) {
		__position.rx() = __myFlightTracker->getTracked()->position.longitude / 180;
		__position.ry() = __myFlightTracker->getTracked()->position.latitude / 90;
	}
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, TEXCOORDS);
	checkGLErrors(HERE);
	__prepareMatrix(WORLD);
	
#ifndef NO_DEBUG
	__drawMarks();
#endif
	
	__drawWorld();
	
	if (__settings->getDisplayLayersPolicy().firs)
		__drawFirs();
	
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	__prepareMatrix(AIRPORTS_PILOTS);
	
	if (__settings->getDisplayLayersPolicy().firs)
		__drawFirsLabels();
	
	if (__settings->getDisplayLayersPolicy().airports)
		__drawAirports();
	
	if (__settings->getDisplayLayersPolicy().pilots)
		__drawPilots();
	
	
	__drawLines();
	
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	if (!__underMouse || __dontDisplayTooltip) {
		QToolTip::hideText();
		if (cursor().shape() != Qt::SizeAllCursor)
			setCursor(QCursor(Qt::ArrowCursor));
	} else
		__drawToolTip();
}

void
MapWidget::resizeGL(int width, int height) {
	glViewport(0, 0, width, height);
	
	__orthoRangeX = (GLdouble)width / BASE_SIZE_WIDTH;
	__orthoRangeY = (GLdouble)height / BASE_SIZE_HEIGHT;
	
	__prepareMatrix(WORLD);
	
	__winWidth = width;
	__winHeight = height;
}

void
MapWidget::wheelEvent(QWheelEvent* _event) {
	int steps = _event->delta() / 120;
	
	if (__zoom + steps <= ZOOM_MINIMUM)
		__zoom = ZOOM_MINIMUM;
	else
		__zoom += steps;
	
	_event->accept();
	
	updateGL();
}

void
MapWidget::mousePressEvent(QMouseEvent* _event) {
	__lastMousePos = _event->pos();
	QToolTip::hideText();
	
	__dontDisplayTooltip = true;
	if ((_event->buttons() & Qt::RightButton) && __underMouse) {
		switch (__underMouse->objectType()) {
			case PLANE:
				emit contextMenuRequested(static_cast< const Pilot* >(__underMouse));
				break;
			case AIRPORT:
				emit contextMenuRequested(static_cast< const AirportObject* >(__underMouse));
				break;
			case FIR:
				emit contextMenuRequested(static_cast< const Fir* >(__underMouse));
				break;
			case UIR:
				break;
		}
		__underMouse = NULL;
	} else if ((_event->buttons() & Qt::LeftButton) && __underMouse) {
		__recentlyClickedMousePos = _event->pos();
	}
}

void
MapWidget::mouseReleaseEvent(QMouseEvent* _event) {
	setCursor(QCursor(Qt::ArrowCursor));
	__lastMousePos = _event->pos();
	if (__underMouse) {
		QToolTip::hideText();
		__dontDisplayTooltip = true;
		if (__recentlyClickedMousePos == __lastMousePos) {
			switch (__underMouse->objectType()) {
				case PLANE:
					emit flightDetailsWindowRequested(static_cast< const Pilot* >(__underMouse));
					break;
				case AIRPORT:
					emit airportDetailsWindowRequested(static_cast< const AirportObject* >(__underMouse));
					break;
				case FIR:
					emit firDetailsWindowRequested(static_cast< const Fir* >(__underMouse));
					break;
				case UIR:
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
		__position.rx() -= (double)dx / 400.0 / (double)__zoom;
		
		double newY = __position.y() + ((double)dy / 300.0 / (double)__zoom);
		if ((newY < RANGE_Y) && (newY > -RANGE_Y))
			__position.setY(newY);
		
		if (__myFlightTracker->getTracked() && (dx != 0 || dy != 0))
			emit flightTrackingCanceled();
	}
	
	__lastMousePos = _event->pos();
	
	// counts the mouse position point, interpolated to the GL's context size
	__lastMousePosInterpolated.rx() = ((double)__lastMousePos.x() -
			(((double)__winWidth - BASE_SIZE_WIDTH) / 2)) / (BASE_SIZE_WIDTH / 2) - 1.0;
	__lastMousePosInterpolated.ry() = -(((double)__lastMousePos.y() -
			(((double)__winHeight -BASE_SIZE_HEIGHT) / 2)) / (BASE_SIZE_HEIGHT / 2) - 1.0);
	
	// count the mouse position global coordinates
	double longitude = (__lastMousePosInterpolated.x() / (double)__zoom + __position.x()) * 180;
	double latitude = (__lastMousePosInterpolated.y() / (double)__zoom + __position.y()) * 90;
	if (latitude < -90)
		latitude = -90;
	else if (latitude > 90)
		latitude = 90;
	
	// update the label on the very bottom of the main window
	UserInterface::GetSingleton().getPositionBox()->setText(
			QString((latitude > 0) ? "N" : "S") + " " +
			QString::number(absHelper(latitude), 'g', 6) + " " + 
			QString((longitude < 0) ? "W" : "E") + " " +
			QString::number(absHelper(longitude), 'g', 6) + " "
		);
	
	__dontDisplayTooltip = false;
	
	updateGL();
}

void
MapWidget::keyPressEvent(QKeyEvent* _event) {
	switch (_event->key()) {
		case Qt::Key_PageUp:
			__zoom += 1;
			break;
		case Qt::Key_PageDown:
			if (__zoom > ZOOM_MINIMUM)
				__zoom -= 1;
			break;
		case Qt::Key_Control:
			__keyPressed = true;
	}
	
	updateGL();
}

void
MapWidget::keyReleaseEvent(QKeyEvent* _event) {
	switch (_event->key()) {
		case Qt::Key_Control:
			__keyPressed = false;
			break;
	}
	
	updateGL();
}

void
MapWidget::__loadNewSettings() {
	if (__settings->hasAntyaliasing())
		__setAntyaliasing(true);
	else
		__setAntyaliasing(false);
}


void
MapWidget::__openContextMenu(const Pilot* _pilot) {
	QMenu* dMenu = new QMenu(_pilot->callsign, this);
	
	ClientDetailsAction* showDetails = new ClientDetailsAction(_pilot, "Flight details", this);
	TrackAction* trackThisFlight = new TrackAction(_pilot, this);
	dMenu->addAction(showDetails);
	dMenu->addAction(trackThisFlight);
	
	connect(showDetails,		SIGNAL(triggered(const Client*)),
		__flightDetailsWindow,	SLOT(showWindow(const Client*)));
	connect(trackThisFlight,	SIGNAL(triggered(const Pilot*)),
		this,			SIGNAL(flightTrackingRequested(const Pilot*)));
	
	dMenu->addSeparator();
	
	if (!_pilot->route.origin.isEmpty()) {
		MetarAction* showDepMetar = new MetarAction(_pilot->route.origin, this);
		dMenu->addAction(showDepMetar);
		connect(showDepMetar,	SIGNAL(triggered(QString)),
			__metarsWindow,	SLOT(showWindow(QString)));
	}
	
	if (!_pilot->route.destination.isEmpty()) {
		MetarAction* showArrMetar = new MetarAction(_pilot->route.destination, this);
		dMenu->addAction(showArrMetar);
		connect(showArrMetar,	SIGNAL(triggered(QString)),
			__metarsWindow,	SLOT(showWindow(QString)));
	}
	
	
	dMenu->exec(mapToGlobal(__lastMousePos));
	delete dMenu;
}

void
MapWidget::__openContextMenu(const AirportObject* _ap) {
	QMenu* dMenu = new QMenu(_ap->getData()->icao, this);
	
	AirportDetailsAction* showAp = new AirportDetailsAction(_ap, "Airport details", this);
	MetarAction* showMetar = new MetarAction(_ap->getData()->icao, this);
	
	dMenu->addAction(showAp);
	dMenu->addAction(showMetar);
	
	connect(showAp,			SIGNAL(triggered(const AirportObject*)),
		__airportDetailsWindow,	SLOT(showWindow(const AirportObject*)));
	
	connect(showMetar,		SIGNAL(triggered(QString)),
		__metarsWindow,		SLOT(showWindow(QString)));
	
	if (!_ap->getStaff().isEmpty()) {
		dMenu->addSeparator();
		dMenu->addAction(new ActionMenuSeparator("Controllers", this));
		
		for (const Controller* c: _ap->getStaff()) {
			ClientDetailsAction* showDetails = new ClientDetailsAction(c, c->callsign, this);
			dMenu->addAction(showDetails);
			connect(showDetails,		SIGNAL(triggered(const Client*)),
				__atcDetailsWindow,	SLOT(showWindow(const Client*)));
		}
	}
	
	if (!_ap->getOutbounds().isEmpty() && _ap->countDepartures()) {
		dMenu->addSeparator();
		dMenu->addAction(new ActionMenuSeparator("Departures", this));
		
		for (const Pilot* p: _ap->getOutbounds()) {
			if (p->flightStatus != DEPARTING)
				continue;
			ClientDetailsAction* showDetails = new ClientDetailsAction(
					p,
					p->callsign % " to " % p->route.destination,
					this
				);
			dMenu->addAction(showDetails);
			connect(showDetails,		SIGNAL(triggered(const Client*)),
				__flightDetailsWindow,	SLOT(showWindow(const Client*)));
		}
	}
	
	if (!_ap->getInbounds().isEmpty() && _ap->countArrivals()) {
		dMenu->addSeparator();
		dMenu->addAction(new ActionMenuSeparator("Arrivals", this));
		
		for (const Pilot* p: _ap->getInbounds()) {
			if (p->flightStatus != ARRIVED)
				continue;
			ClientDetailsAction* showDetails = new ClientDetailsAction(
					p,
					p->callsign % " from " % p->route.origin,
					this
				);
			dMenu->addAction(showDetails);
			connect(showDetails,		SIGNAL(triggered(const Client*)),
				__flightDetailsWindow,	SLOT(showWindow(const Client*)));
		}
	}
	
	dMenu->exec(mapToGlobal(__lastMousePos));
	delete dMenu;
}

void
MapWidget::__openContextMenu(const Fir* _fir) {
	QMenu* dMenu = new QMenu(_fir->header.icao, this);
	
	FirDetailsAction* showFir = new FirDetailsAction(_fir,
		static_cast< QString >(_fir->header.icao).simplified() % " details", this);
	
	dMenu->addAction(showFir);
	
	connect(showFir,		SIGNAL(triggered(const Fir*)),
		__firDetailsWindow,	SLOT(showWindow(const Fir*)));
	
	
	for (const Controller* c: _fir->getStaff()) {
		ClientDetailsAction* showDetails = new ClientDetailsAction(c, c->callsign, this);
		dMenu->addAction(showDetails);
		connect(showDetails,		SIGNAL(triggered(const Client*)),
			__atcDetailsWindow,	SLOT(showWindow(const Client*)));
	}
	
	for (const Controller* c: _fir->getUirStaff()) {
		ClientDetailsAction* showDetails = new ClientDetailsAction(c, c->callsign, this);
		dMenu->addAction(showDetails);
		connect(showDetails,		SIGNAL(triggered(const Client*)),
			__atcDetailsWindow,	SLOT(showWindow(const Client*)));
	}
	
	dMenu->exec(mapToGlobal(__lastMousePos));
	delete dMenu;
}

void
MapWidget::__init() {
	setEnabled(true);
	
#ifndef NO_DEBUG
	qDebug() << "Loading images...";
#endif
	__apIcon = loadImage(":/pixmaps/airport.png");
	__apStaffedIcon = loadImage(":/pixmaps/airport_staffed.png");
	__pilotIcon = loadImage(":/pixmaps/plane.png");

#ifndef NO_DEBUG
	qDebug() << "Getting pointers...";
#endif
	__firs = FirsDatabase::GetSingletonPtr();
	__airportDetailsWindow = AirportDetailsWindow::GetSingletonPtr();
	__atcDetailsWindow = ATCDetailsWindow::GetSingletonPtr();
	__metarsWindow = MetarsWindow::GetSingletonPtr();
	__firDetailsWindow = FirDetailsWindow::GetSingletonPtr();
	__flightDetailsWindow = FlightDetailsWindow::GetSingletonPtr();
	__settings = SettingsManager::GetSingletonPtr();
	__myWorldMap = WorldMap::GetSingletonPtr();
	
	__myFlightTracker = FlightTracker::GetSingletonPtr();
	
#ifndef NO_DEBUG
	qDebug() << "Preparing slots...";
#endif
	connect(this,			SIGNAL(firDetailsWindowRequested(const Fir*)),
		__firDetailsWindow,	SLOT(showWindow(const Fir*)));
	connect(this,			SIGNAL(flightDetailsWindowRequested(const Client*)),
		__flightDetailsWindow,	SLOT(showWindow(const Client*)));
	connect(this,			SIGNAL(airportDetailsWindowRequested(const AirportObject*)),
		__airportDetailsWindow,	SLOT(showWindow(const AirportObject*)));
	connect(__settings,		SIGNAL(settingsChanged()),
		this,			SLOT(__loadNewSettings()));
	
#ifndef NO_DEBUG
	qDebug() << "Setting fonts...";
#endif
	__pilotFont.setPixelSize(PILOT_FONT_PIXEL_SIZE);
	__pilotFont.setWeight(PILOT_FONT_WEIGHT);
	
	__airportFont.setPixelSize(AIRPORT_FONT_PIXEL_SIZE);
	__airportFont.setWeight(AIRPORT_FONT_WEIGHT);
	
#ifndef NO_DEBUG
	qDebug() << "Restoring settings...";
#endif
	__restoreSettings();
	__loadNewSettings();
	
#ifndef NO_DEBUG
	qDebug() << "Generating pixmaps...";
#endif
	
	__firs->init();
	WorldMap::GetSingleton().init();
}

void
MapWidget::__prepareMatrix(PMMatrixMode _mode) {
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
			
			glScalef(1.0f/180.0f, 1.0f/90.0f, 1.0f);
			glScalef(__zoom, __zoom, 1.0);
 			glTranslated(-__position.x() * 180, -__position.y() * 90, 0.0);
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
		glVertex2f(-168.010255,65.658275);
		
		// Jastrzębia Góra, Poland
		glVertex2f(18.316498, 54.830754);
		
		// the most eastern coast of Russia :)
		glVertex2f(-169.764405,66.10717);
		
		// Tierra del Fuego, Argentina
		glVertex2f(-65.166321,-54.664301);
		
		// Aghulas National Park, Republic of South Africa
		glVertex2f(20.000153,-34.827332);
		
		// Steward Island, New Zealand
		glVertex2f(167.705383,-47.118738);
	
	glEnd();
	checkGLErrors(HERE);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, TEXCOORDS);checkGLErrors(HERE);
}
#endif


void
MapWidget::__drawWorld() {
	glPushMatrix();
		glTranslatef(0.0, 0.0, -0.9);
		
		qglColor(__settings->getLandsColor());
		__myWorldMap->draw();
		checkGLErrors(HERE);
	glPopMatrix();
}

void
MapWidget::__drawFirs() {
	glPushMatrix();
		glTranslatef(0.0, 0.0, -0.8);
		for (const Fir& fir: __firs->getFirs()) {
			if (fir.isStaffed()) {
				continue;
			}
			
			qglColor(__settings->getUnstaffedFirBordersColor());		
			fir.drawBorders(); checkGLErrors(HERE);
		}
	glPopMatrix();
	
	if (__settings->getDisplayLayersPolicy().uirs)
		__drawUirs();
	
	glLineWidth(3.0);
	glPushMatrix();
		glTranslatef(0.0, 0.0, -0.6);
		
		for (const Fir& fir: __firs->getFirs()) {
			if (!fir.isStaffed())
				continue;
			
			qglColor(__settings->getStaffedFirBordersColor());
			fir.drawBorders(); checkGLErrors(HERE);
			
			qglColor(__settings->getStaffedFirBackgroundColor());
			fir.drawTriangles(); checkGLErrors(HERE);
		}
	glPopMatrix();
	glLineWidth(1.0);
}

void
MapWidget::__drawUirs() {
	glLineWidth(3.0);
	
	glPushMatrix();
		glTranslatef(0.0, 0.0, -0.7);
		for (const Uir* uir: __data.getUIRs()) {
			if (!uir->getStaff().isEmpty()) {
				for (const Fir* fir: uir->getRange()) {
					qglColor(__settings->getStaffedUirBordersColor());
					fir->drawBorders(); checkGLErrors(HERE);
					
					qglColor(__settings->getStaffedUirBackgroundColor());
					fir->drawTriangles(); checkGLErrors(HERE);
				}
			}
			checkGLErrors(HERE);
		}
	
	glPopMatrix();
	
	qglColor(__settings->getSeasColor());
	glLineWidth(1.0);
	checkGLErrors(HERE);
}

void
MapWidget::__drawFirsLabels() {
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glPushMatrix();
		glTranslatef(0.0, 0.0, -0.5);
		for (const Fir& fir: __firs->getFirs()) {
			float x, y;
			__mapCoordinates(fir.header.textPosition.x, fir.header.textPosition.y, x, y);
			if ((x <= __orthoRangeX) && (y <= __orthoRangeY) &&
					(x >= -__orthoRangeX) && (y >= -__orthoRangeY)) {
			
				drawFirLabel(x, y, fir);
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
MapWidget::__drawAirports() {
	for (auto it = __airports.begin(); it != __airports.end(); ++it) {
		
		if (!it.value()->getData())
			continue;
		
		GLfloat x = ((it.value()->getData()->longitude / 180) - __position.x()) * __zoom;
		if (x < -__orthoRangeX || x > __orthoRangeX)
			continue;
		
		GLfloat y = ((it.value()->getData()->latitude / 90) - __position.y()) * __zoom;
		if (y < -__orthoRangeY || y > __orthoRangeY)
			continue;
		
		glVertexPointer(2, GL_FLOAT, 0, VERTICES); checkGLErrors(HERE);
		
		bool inRange = __distanceFromCamera(x, y) < OBJECT_TO_MOUSE;
		
		glBindTexture(GL_TEXTURE_2D, (it.value()->getStaff().isEmpty()) ? __apIcon : __apStaffedIcon );
		checkGLErrors(HERE);
		
		glPushMatrix();
		
			glColor4f(1.0, 1.0, 1.0, 1.0);
			
			glTranslatef(x, y, -0.4); checkGLErrors(HERE);
			glDrawArrays(GL_QUADS, 0, 4); checkGLErrors(HERE);
			
			drawIcaoLabel(it.value()); checkGLErrors(HERE);
			
			if (inRange && !__underMouse) {
				__underMouse = it.value();
			}
			
			if (it.value()->hasApproach()) {
				glBindTexture(GL_TEXTURE_2D, 0); checkGLErrors(HERE);
				
				qglColor(__settings->getApproachCircleColor());
				glVertexPointer(2, GL_FLOAT, 0, __circle); checkGLErrors(HERE);
				
				glLineWidth(1.5);
				glLineStipple(1, 0xF0F0);
				glPushMatrix();
					glScalef(0.005f * __zoom, 0.005f * __zoom, 0); checkGLErrors(HERE);
					glDrawArrays(GL_LINE_LOOP, 0, __circleCount);
				glPopMatrix();
				glLineWidth(1.0);
				glLineStipple(1, 0xFFFF); checkGLErrors(HERE);
				
				glVertexPointer(2, GL_FLOAT, 0, VERTICES);
			}
			
		glPopMatrix();
	}
	
	glBindTexture(GL_TEXTURE_2D, 0); checkGLErrors(HERE);
}

void
MapWidget::__drawPilots() {
	const QVector< Pilot* > & pilots = VatsimDataHandler::GetSingleton().getPilots();
	
	glColor4f(1.0, 1.0, 1.0, 1.0);
	
	
	for (const Pilot* client: pilots) {
		if (client->flightStatus != AIRBORNE)
			continue;
		
		GLfloat x = client->position.longitude / 180;
		x -= __position.x();
		x *= __zoom;
		
		if (x < -__orthoRangeX || x > __orthoRangeX)
			continue;
		
		GLfloat y = client->position.latitude / 90;
		y -= __position.y();
		y *= __zoom;
		
		if (y < -__orthoRangeY || y > __orthoRangeY)
			continue;
		
		glVertexPointer(2, GL_FLOAT, 0, VERTICES); checkGLErrors(HERE);
		glBindTexture(GL_TEXTURE_2D, __pilotIcon); checkGLErrors(HERE);
		
		glPushMatrix();
			bool inRange = __distanceFromCamera(x, y) < OBJECT_TO_MOUSE;
			
			glColor4f(1.0, 1.0, 1.0, 1.0);
			
			glTranslatef(x, y, -0.2); checkGLErrors(HERE);
			
			glPushMatrix();
				glRotatef((GLfloat)client->heading, 0, 0, -1); checkGLErrors(HERE);
				
				if (inRange && !__underMouse)
					glScalef(1.3, 1.3, 1.0); checkGLErrors(HERE);
				
				glDrawArrays(GL_QUADS, 0, 4); checkGLErrors(HERE);
				
			glPopMatrix();
			
			if (((__settings->displayPilotsLabelsWhenHovered())
					&& (__keyPressed || inRange))
					|| (__settings->displayPilotsLabelsAlways()))
				drawCallsign(client);
			
			if (inRange && !__underMouse) {
				__underMouse = client;
				
			}
		glPopMatrix(); checkGLErrors(HERE);
	}
	
	glBindTexture(GL_TEXTURE_2D, 0); checkGLErrors(HERE);
}

void
MapWidget::__drawLines() {
	if (!__underMouse)
		return;

	if (__underMouse->objectType() == PLANE) {
		glColor4f(LINES_COLOR);
		const Pilot* pilot = static_cast< const Pilot* >(__underMouse);
		const AirportRecord* ap;
		if (!pilot->route.origin.isEmpty())
			ap = __airports[pilot->route.origin]->getData();
		else
			ap = NULL;
		
		if (ap) {
			GLfloat vertices[] = {
					((GLfloat)(ap->longitude / 180 - __position.x()) * __zoom),
					((GLfloat)(ap->latitude / 90 - __position.y()) * __zoom),
					((GLfloat)(pilot->position.longitude / 180 - __position.x()) * __zoom),
					((GLfloat)(pilot->position.latitude / 90 - __position.y()) * __zoom)
				};
			
			glVertexPointer(2, GL_FLOAT, 0, vertices); checkGLErrors(HERE);
			glDrawArrays(GL_LINES, 0, 2); checkGLErrors(HERE);
		}
		
		if (!pilot->route.destination.isEmpty())
			ap = __airports[pilot->route.destination]->getData();
		else
			ap = NULL;
		
		if (ap) {
			GLfloat vertices[] = {
					((GLfloat)(ap->longitude / 180 - __position.x()) * __zoom),
					((GLfloat)(ap->latitude / 90 - __position.y()) * __zoom),
					((GLfloat)(pilot->position.longitude / 180 - __position.x()) * __zoom),
					((GLfloat)(pilot->position.latitude / 90 - __position.y()) * __zoom)
				};
				
			glVertexPointer(2, GL_FLOAT, 0, vertices); checkGLErrors(HERE);
			glLineStipple(1, 0xF0F0);
			glDrawArrays(GL_LINES, 0, 2); checkGLErrors(HERE);
			glLineStipple(1, 0xFFFF);
			
		}
	} else if (__underMouse->objectType() == AIRPORT) {
		const AirportObject* ap = static_cast< const AirportObject* >(__underMouse);
		if (!ap->getData())
			return;
		
		unsigned linesOut = ap->getOutbounds().size() - ap->countDepartures();
		unsigned linesIn = ap->getInbounds().size() - ap->countArrivals();
		
		GLfloat* vertices = new GLfloat[(linesOut + linesIn) * 4];
		unsigned i = 0;
		for (const Pilot* p: ap->getOutbounds()) {
			if (p->flightStatus == DEPARTING)
				continue;
			
			/* We cannot use vertices[i++], vertices[i++], as the result of such
			 * called function is undefined (according to C++ standard) */
			__mapCoordinates(p->position.longitude, p->position.latitude,
					 vertices[i], vertices[i+1]);
			if ((__settings->displayPilotsLabelsAirportRelated())
					&& (p->flightStatus != ARRIVED && !__keyPressed))
				drawCallsign(vertices[i], vertices[i+1], p); checkGLErrors(HERE);
			i += 2;
			
			__mapCoordinates(ap->getData()->longitude, ap->getData()->latitude,
					 vertices[i], vertices[i+1]);
			
			i += 2;
		}
		for (const Pilot* p: ap->getInbounds()) {
			if (p->flightStatus == ARRIVED)
				continue;
			
			__mapCoordinates(p->position.longitude, p->position.latitude,
					 vertices[i], vertices[i+1]);
			
			if ((__settings->displayPilotsLabelsAirportRelated())
				&& (p->flightStatus != DEPARTING && !__keyPressed))
				drawCallsign(vertices[i], vertices[i+1], p); checkGLErrors(HERE);
			i += 2;
			
			__mapCoordinates(ap->getData()->longitude, ap->getData()->latitude,
							 vertices[i], vertices[i+1]);
			
			i += 2;
		}
		
		glVertexPointer(2, GL_FLOAT, 0, vertices); checkGLErrors(HERE);
		glColor4f(LINES_COLOR);
		
		glDrawArrays(GL_LINES, 0, linesOut * 2); checkGLErrors(HERE);
		
		glLineStipple(1, 0xF0F0);
		glDrawArrays(GL_LINES, linesOut * 2, linesIn * 2); checkGLErrors(HERE);
		glLineStipple(1, 0xFFFF);
		
		delete [] vertices;
	}
}

void
MapWidget::__drawToolTip() {
	setCursor(QCursor(Qt::PointingHandCursor));
	
	QString text;
	switch (__underMouse->objectType()) {
		case PILOT:
			text = __producePilotToolTip(static_cast< const Pilot* >(__underMouse));
			break;
		case AIRPORT:
			text = __produceAirportToolTip(static_cast< const AirportObject* >(__underMouse));
			break;
		case FIR:
			text = __produceFirToolTip(static_cast< const Fir* >(__underMouse));
			break;
		case UIR:
			break;
	}
	
	QToolTip::showText(mapToGlobal(__lastMousePos), text, this);
	
}

void
MapWidget::__setAntyaliasing(bool _on) {
	if (_on) {
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST); checkGLErrors(HERE);
		glEnable(GL_LINE_SMOOTH);checkGLErrors(HERE);
	} else {
		glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST); checkGLErrors(HERE);
		glDisable(GL_LINE_SMOOTH); checkGLErrors(HERE);
	}
}

void
MapWidget::__storeSettings() {
	QSettings settings("Vatsinator", "Vatsinator");
	
	settings.beginGroup("CameraSettings");
	
	settings.setValue("zoomFactor", __zoom);
	settings.setValue("cameraPosition", __position);
	
	settings.endGroup();
}

void
MapWidget::__restoreSettings() {
	QSettings settings("Vatsinator", "Vatsinator");
	
	settings.beginGroup("CameraSettings");
	
	__zoom = settings.value("zoomFactor", ZOOM_MINIMUM).toInt();
	__position = settings.value("cameraPosition", QPointF(0.0, 0.0)).toPointF();

	settings.endGroup();
}

void
MapWidget::__produceCircle() {
	__circleCount = 0;
	for (double angle = 0.0; angle <= (2 * PI); angle += 0.1, ++__circleCount)
		{} // count how many vertices we will have
	
	__circle = new GLfloat[__circleCount * 2 + 2];
	unsigned i = 0;
	
	float x, y;
	for (double angle = 0.0; angle <= (2 * PI); angle += 0.1) {
		x = cos(angle);
		y = sin(angle);
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
	return (QString)
		"<center>" %
		_p->callsign % "<br><nobr>" %
		_p->realName % " (" % _p->aircraft % ")</nobr><br><nobr>" %
		(_p->route.origin.isEmpty() ? "(unknown)" : (__airports[_p->route.origin]->getData() ?
				_p->route.origin % " " % (QString)__airports[_p->route.origin]->getData()->city :
				_p->route.origin)) %
		" > " %
		(_p->route.destination.isEmpty() ? "(unknown)" : (__airports[_p->route.destination]->getData() ?
				_p->route.destination % " " % (QString)__airports[_p->route.destination]->getData()->city :
				_p->route.destination)) %
		"</nobr><br>" %
		"Ground speed: " % QString::number(_p->groundSpeed) % " kts<br>Altitude: " %
		QString::number(_p->altitude) % " ft</center>";
}

inline QString
MapWidget::__produceAirportToolTip(const AirportObject* _ap) {
	QString text = (QString)"<center>" % (QString)_ap->getData()->icao % "<br><nobr>" %
		(QString)_ap->getData()->name % ", " %
		(QString)_ap->getData()->city % "</nobr>";
	
	for (const Controller* c: _ap->getStaff())
		text.append((QString)"<br><nobr>" %
			c->callsign % " " % c->frequency % " " % c->realName %
			"</nobr>"
		);
	
	int deps = _ap->countDepartures();
	if (deps)
		text.append((QString)"<br>Departures: " % QString::number(deps));
	
	int arrs = _ap->countArrivals();
	if (arrs)
		text.append((QString)"<br>Arrivals: " % QString::number(arrs));
	
	text.append("</center>");
	return text;
}

inline QString
MapWidget::__produceFirToolTip(const Fir* _f) {
	if (_f->name.isEmpty() && _f->getStaff().isEmpty() && _f->getUirStaff().isEmpty())
		return "";
	
	QString text = (QString)"<center>";
	if (!_f->name.isEmpty()) {
		text.append((QString)"<nobr>" % _f->name);
		if (!_f->country.isEmpty())
			text.append((QString)", " % _f->country);
		text.append((QString)"</nobr>");
	}
	
	for (const Controller* c: _f->getStaff())
		text.append((QString)"<br><nobr>" %
			c->callsign % " " % c->frequency % " " % c->realName %
			"</nobr>"
		);
	
	for (const Controller* c: _f->getUirStaff())
		text.append((QString)"<br><nobr>" %
			c->callsign % " " % c->frequency % " " % c->realName %
			"</nobr>"
		);
	
	text.append("</center>");
	return text;
}
