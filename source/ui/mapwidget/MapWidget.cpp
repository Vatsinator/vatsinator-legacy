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
#include <cstdlib>

#include "db/AirportsDatabase.h"
#include "db/FirsDatabase.h"

#include "settings/SettingsManager.h"

#include "ui/UserInterface.h"
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

#include "VatsinatorApplication.h"

#include "MapWidget.h"

#include "defines.h"

const double PI = 3.1415926535897;

const GLdouble VERTICES[] = {
	-0.025, -0.025,
	-0.025,  0.025,
	 0.025,  0.025,
	 0.025, -0.025 };
const GLdouble PILOT_TOOLTIP_VERTICES[] = {
	-0.09375,  0.02,
	-0.09375,  0.1,
	 0.09375,  0.1,
	 0.09375,  0.02 };
const GLdouble AIRPORT_TOOLTIP_VERTICES[] = {
	-0.06, -0.065,
	-0.06, -0.025,
	 0.06, -0.025,
	 0.06, -0.065 };
const GLdouble FIR_TOOLTIP_VERTICES[] = {
	-0.08, -0.04,
	-0.08,  0.04,
	 0.08,  0.04,
	 0.08, -0.04 };
const GLdouble TEXCOORDS[] = {
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
		glTranslatef(0.0f, 0.0f, -0.1f);
		glBindTexture(GL_TEXTURE_2D, _p->callsignTip);
		glVertexPointer(2, GL_DOUBLE, 0, PILOT_TOOLTIP_VERTICES);
		glDrawArrays(GL_QUADS, 0, 4);
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}

inline void drawCallsign(GLdouble _x, GLdouble _y, const Pilot* _p) {
	glPushMatrix();
		glTranslated(_x, _y, -0.1);
		glBindTexture(GL_TEXTURE_2D, _p->callsignTip);
		glVertexPointer(2, GL_DOUBLE, 0, PILOT_TOOLTIP_VERTICES);
		glDrawArrays(GL_QUADS, 0, 4);
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}

inline void drawIcaoLabel(const AirportObject* _ap) {
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, -0.1f);
		glBindTexture(GL_TEXTURE_2D, _ap->labelTip);
		glVertexPointer(2, GL_DOUBLE, 0, AIRPORT_TOOLTIP_VERTICES);
		glDrawArrays(GL_QUADS, 0, 4);
		glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
}

inline void drawFirLabel(GLdouble _x, GLdouble _y, const Fir& _f) {
	if (_f.icaoTip) {
		glPushMatrix();
			glTranslated(_x, _y, 0.0);
			glBindTexture(GL_TEXTURE_2D, _f.icaoTip);
			glVertexPointer(2, GL_DOUBLE, 0, FIR_TOOLTIP_VERTICES);
			glDrawArrays(GL_QUADS, 0, 4);
			glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();
	}
}





MapWidget::MapWidget(QWidget* _parent) :
		QGLWidget(_parent),
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
		__tracked(NULL),
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

void
MapWidget::init() {
	setEnabled(true);
	
	__apIcon = loadImage(":/pixmaps/airport.png");
	__apStaffedIcon = loadImage(":/pixmaps/airport_staffed.png");
	__pilotIcon = loadImage(":/pixmaps/plane.png");
	
	__firs = FirsDatabase::GetSingletonPtr();
	__airportDetailsWindow = AirportDetailsWindow::GetSingletonPtr();
	__atcDetailsWindow = ATCDetailsWindow::GetSingletonPtr();
	__metarsWindow = MetarsWindow::GetSingletonPtr();
	__firDetailsWindow = FirDetailsWindow::GetSingletonPtr();
	__flightDetailsWindow = FlightDetailsWindow::GetSingletonPtr();
	__settings = SettingsManager::GetSingletonPtr();
	
	connect(this,			SIGNAL(firDetailsWindowRequested(const Fir*)),
		__firDetailsWindow,	SLOT(showWindow(const Fir*)));
	connect(this,			SIGNAL(flightDetailsWindowRequested(const Client*)),
		__flightDetailsWindow,	SLOT(showWindow(const Client*)));
	connect(this,			SIGNAL(airportDetailsWindowRequested(const AirportObject*)),
		__airportDetailsWindow,	SLOT(showWindow(const AirportObject*)));
	
	__pilotFont.setPixelSize(PILOT_FONT_PIXEL_SIZE);
	__pilotFont.setWeight(PILOT_FONT_WEIGHT);
	
	__airportFont.setPixelSize(AIRPORT_FONT_PIXEL_SIZE);
	__airportFont.setWeight(AIRPORT_FONT_WEIGHT);
	
	__firs->prepareTooltips();
	
	__restoreSettings();
}

void
MapWidget::initializeGL() {
	glShadeModel(GL_SMOOTH);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glEnable(GL_LINE_STIPPLE);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);
	glEnable(GL_DEPTH_TEST);
	
	init();
}

void
MapWidget::paintGL() {
	__underMouse = NULL;
	
	qglColor(__settings->getBackgroundColor());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	qglClearColor(__settings->getBackgroundColor());
	glLoadIdentity();
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_DEPTH_TEST);
	
	if (__tracked) {
		__position.rx() = __tracked->position.longitude / 180;
		__position.ry() = __tracked->position.latitude / 90;
	}
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_DOUBLE, 0, TEXCOORDS);
	
	__prepareMatrix(WORLD);
	
	if (__settings->getDisplayLayersPolicy().firs)
		__drawFirs();
	
	
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

GLuint
MapWidget::loadImage(const QImage& _img) {
	GLuint pix;
	QImage final = QGLWidget::convertToGLFormat(_img);
	
	glGenTextures(1, &pix);
	
	glBindTexture(GL_TEXTURE_2D, pix);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, final.width(), final.height(),
			0, GL_RGBA, GL_UNSIGNED_BYTE, final.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_LINEAR causes the text
									// on the tooltip is blurred
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
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
	
	glEnable(GL_TEXTURE_2D);
	
	glGenTextures(1, &pix);
	
	glBindTexture(GL_TEXTURE_2D, pix);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, final.width(), final.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, final.bits());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return pix;
}

void
MapWidget::deleteImage(GLuint _tex) {
	glDeleteTextures(1, &_tex);
}

void
MapWidget::showPilot(const Pilot* _p) {
	__position.rx() = _p->position.longitude / 180;
	__position.ry() = _p->position.latitude / 90;
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
	}
}

void
MapWidget::mouseReleaseEvent(QMouseEvent* _event) {
	setCursor(QCursor(Qt::ArrowCursor));
	__lastMousePos = _event->pos();
	if (__underMouse) {
		QToolTip::hideText();
		__dontDisplayTooltip = true;
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
	__underMouse = NULL;
}

void
MapWidget::mouseMoveEvent(QMouseEvent* _event) {
	int dx = _event->x() - __lastMousePos.x();
	int dy = _event->y() - __lastMousePos.y();
	
	if (_event->buttons() & Qt::LeftButton) {
		setCursor(QCursor(Qt::SizeAllCursor));
		__tracked = NULL;
		
		// count the new position
		__position.rx() -= (double)dx / 400.0 / (double)__zoom;
		
		double newY = __position.y() + ((double)dy / 300.0 / (double)__zoom);
		if ((newY < RANGE_Y) && (newY > -RANGE_Y))
			__position.setY(newY);
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
MapWidget::__openContextMenu(const Pilot* _pilot) {
	QMenu* dMenu = new QMenu(_pilot->callsign, this);
	
	ClientDetailsAction* showDetails = new ClientDetailsAction(_pilot, "Flight details", this);
	TrackAction* trackThisFlight = new TrackAction(_pilot, this);
	dMenu->addAction(showDetails);
	dMenu->addAction(trackThisFlight);
	
	connect(showDetails,		SIGNAL(clicked(const Client*)),
		__flightDetailsWindow,	SLOT(showWindow(const Client*)));
	connect(trackThisFlight,	SIGNAL(clicked(const Pilot*)),
		this,			SLOT(trackFlight(const Pilot*)));
	
	dMenu->addSeparator();
	
	if (!_pilot->route.origin.isEmpty()) {
		MetarAction* showDepMetar = new MetarAction(_pilot->route.origin, this);
		dMenu->addAction(showDepMetar);
		connect(showDepMetar,	SIGNAL(clicked(QString)),
			__metarsWindow,	SLOT(showWindow(QString)));
	}
	
	if (!_pilot->route.destination.isEmpty()) {
		MetarAction* showArrMetar = new MetarAction(_pilot->route.destination, this);
		dMenu->addAction(showArrMetar);
		connect(showArrMetar,	SIGNAL(clicked(QString)),
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
	
	connect(showAp,			SIGNAL(clicked(const AirportObject*)),
		__airportDetailsWindow,	SLOT(showWindow(const AirportObject*)));
	
	connect(showMetar,		SIGNAL(clicked(QString)),
		__metarsWindow,		SLOT(showWindow(QString)));
	
	if (!_ap->getStaff().isEmpty()) {
		dMenu->addSeparator();
		QAction* desc = new QAction("Controllers", this);
		desc->setEnabled(false);
		dMenu->addAction(desc);
		
		for (const Controller* c: _ap->getStaff()) {
			ClientDetailsAction* showDetails = new ClientDetailsAction(c, c->callsign, this);
			dMenu->addAction(showDetails);
			connect(showDetails,		SIGNAL(clicked(const Client*)),
				__atcDetailsWindow,	SLOT(showWindow(const Client*)));
		}
	}
	
	if (!_ap->getOutbounds().isEmpty() && _ap->countDepartures()) {
		dMenu->addSeparator();
		QAction* desc = new QAction("Departures", this);
		desc->setEnabled(false);
		dMenu->addAction(desc);
		
		for (const Pilot* p: _ap->getOutbounds()) {
			if (p->flightStatus != DEPARTING)
				continue;
			ClientDetailsAction* showDetails = new ClientDetailsAction(p, p->callsign, this);
			dMenu->addAction(showDetails);
			connect(showDetails,		SIGNAL(clicked(const Client*)),
				__flightDetailsWindow,	SLOT(showWindow(const Client*)));
		}
	}
	
	if (!_ap->getInbounds().isEmpty() && _ap->countArrivals()) {
		dMenu->addSeparator();
		QAction* desc = new QAction("Arrivals", this);
		desc->setEnabled(false);
		dMenu->addAction(desc);
		
		for (const Pilot* p: _ap->getInbounds()) {
			if (p->flightStatus != ARRIVED)
				continue;
			ClientDetailsAction* showDetails = new ClientDetailsAction(p, p->callsign, this);
			dMenu->addAction(showDetails);
			connect(showDetails,		SIGNAL(clicked(const Client*)),
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
	
	connect(showFir,		SIGNAL(clicked(const Fir*)),
		__firDetailsWindow,	SLOT(showWindow(const Fir*)));
	
	
	for (const Controller* c: _fir->getStaff()) {
		ClientDetailsAction* showDetails = new ClientDetailsAction(c, c->callsign, this);
		dMenu->addAction(showDetails);
		connect(showDetails,		SIGNAL(clicked(const Client*)),
			__atcDetailsWindow,	SLOT(showWindow(const Client*)));
	}
	
	for (const Controller* c: _fir->getUirStaff()) {
		ClientDetailsAction* showDetails = new ClientDetailsAction(c, c->callsign, this);
		dMenu->addAction(showDetails);
		connect(showDetails,		SIGNAL(clicked(const Client*)),
			__atcDetailsWindow,	SLOT(showWindow(const Client*)));
	}
	
	dMenu->exec(mapToGlobal(__lastMousePos));
	delete dMenu;
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
		 1.0, -1.0
	);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	switch(_mode) {
		case WORLD:
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			
			glScaled((double)1/(double)180, (double)1/(double)90, 1.0);
			glScaled(__zoom, __zoom, 1.0);
 			glTranslated(-__position.x() * 180, -__position.y() * 90, 0.9);
			
			break;
		case AIRPORTS_PILOTS:
			break;
	}
}

void
MapWidget::__drawFirs() {
	glLineWidth(3.0);
	glPushMatrix();
		glTranslatef(0.0, 0.0, -0.2);
		
		for (const Fir& fir: __firs->getFirs()) {
			if (fir.getStaff().isEmpty())
			continue;
			
			qglColor(__settings->getStaffedFirBordersColor());
			glVertexPointer(2, GL_DOUBLE, 0, &fir.coords[0].x);
			glDrawArrays(GL_LINE_LOOP, 0, fir.coords.size());
		}
	glPopMatrix();
	glLineWidth(1.0);
	
	if (__settings->getDisplayLayersPolicy().uirs)
		__drawUirs();
	
	for (const Fir& fir: __firs->getFirs()) {
		if (!fir.getStaff().isEmpty()) {
			continue;
		}
		
		qglColor(__settings->getUnstaffedFirBordersColor());		
		glVertexPointer(2, GL_DOUBLE, 0, &fir.coords[0].x);
		glDrawArrays(GL_LINE_LOOP, 0, fir.coords.size());
	}
}

void
MapWidget::__drawUirs() {
	for (const Uir* uir: __data.getUIRs()) {
		if (!uir->getStaff().isEmpty()) {
			glPushMatrix();
 				glTranslatef(0.0, 0.0, -0.1);
				qglColor(__settings->getStaffedUirBordersColor());
				glLineWidth(3.0);
				for (const Fir* fir: uir->getRange()) {
					glVertexPointer(2, GL_DOUBLE, 0, &fir->coords[0].x);
					glDrawArrays(GL_LINE_LOOP, 0, fir->coords.size());
				}
				glLineWidth(1.0);
			glPopMatrix();
		}
	}
}

void
MapWidget::__drawFirsLabels() {
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glPushMatrix();
		glTranslatef(0.0, 0.0, 0.8);
		for (const Fir& fir: __firs->getFirs()) {
			double x, y;
			__mapCoordinates(fir.header.textPosition.x, fir.header.textPosition.y, x, y);
			if ((x <= __orthoRangeX) && (y <= __orthoRangeY) &&
					(x >= -__orthoRangeX) && (y >= -__orthoRangeY)) {
			
				drawFirLabel(x, y, fir);
				
				if (__distanceFromCamera(x, y) < OBJECT_TO_MOUSE &&
						!__underMouse) {
					__underMouse = &fir;
				}
			}
		}
	glPopMatrix();
}

void
MapWidget::__drawAirports() {
	for (auto it = __airports.begin(); it != __airports.end(); ++it) {
		
		if (!it.value()->getData())
			continue;
		
		GLdouble x = ((it.value()->getData()->longitude / 180) - __position.x()) * __zoom;
		if (x < -__orthoRangeX || x > __orthoRangeX)
			continue;
		
		GLdouble y = ((it.value()->getData()->latitude / 90) - __position.y()) * __zoom;
		if (y < -__orthoRangeY || y > __orthoRangeY)
			continue;
		
		glVertexPointer(2, GL_DOUBLE, 0, VERTICES);
		
		bool inRange = __distanceFromCamera(x, y) < OBJECT_TO_MOUSE;
		
		glBindTexture(GL_TEXTURE_2D, (it.value()->getStaff().isEmpty()) ? __apIcon : __apStaffedIcon );
		
		glPushMatrix();
		
			glColor4f(1.0, 1.0, 1.0, 1.0);
			
 			glTranslated(x, y, 0.7);
			glDrawArrays(GL_QUADS, 0, 4);
			
			drawIcaoLabel(it.value());
			
			if (inRange && !__underMouse) {
				__underMouse = it.value();
			}
			
			if (it.value()->hasApproach()) {
				glBindTexture(GL_TEXTURE_2D, 0);
				
				qglColor(__settings->getApproachCircleColor());
				glVertexPointer(2, GL_DOUBLE, 0, __circle);
				
				glLineWidth(1.5);
				glLineStipple(1, 0xF0F0);
				glPushMatrix();
					glScaled(0.005 * __zoom, 0.005 * __zoom, 0);
					glDrawArrays(GL_LINE_LOOP, 0, __circleCount);
				glPopMatrix();
				glLineWidth(1.0);
				glLineStipple(1, 0xFFFF);
				
				glVertexPointer(2, GL_DOUBLE, 0, VERTICES);
			}
			
		glPopMatrix();
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);
}

void
MapWidget::__drawPilots() {
	const QVector< Pilot* > & pilots = VatsimDataHandler::GetSingleton().getPilots();
	
	glColor4f(1.0, 1.0, 1.0, 1.0);
	
	
	for (const Pilot* client: pilots) {
		if (client->flightStatus != AIRBORNE)
			continue;
		
		GLdouble x = client->position.longitude / 180;
		x -= __position.x();
		x *= __zoom;
		
		if (x < -__orthoRangeX || x > __orthoRangeX)
			continue;
		
		GLdouble y = client->position.latitude / 90;
		y -= __position.y();
		y *= __zoom;
		
		if (y < -__orthoRangeY || y > __orthoRangeY)
			continue;
		
		glVertexPointer(2, GL_DOUBLE, 0, VERTICES);
		glBindTexture(GL_TEXTURE_2D, __pilotIcon);
		
		glPushMatrix();
			bool inRange = __distanceFromCamera(x, y) < OBJECT_TO_MOUSE;
			
			glColor4f(1.0, 1.0, 1.0, 1.0);
			
			glTranslated(x, y, 0.5);
			
			glPushMatrix();
				glRotatef((GLfloat)client->heading, 0, 0, -1);
				
				if (inRange && !__underMouse)
					glScalef(1.3, 1.3, 1.0);
				
				glDrawArrays(GL_QUADS, 0, 4);
				
			glPopMatrix();
			
			if (((__settings->displayPilotsLabelsWhenHovered())
					&& (__keyPressed || inRange))
					|| (__settings->displayPilotsLabelsAlways()))
				drawCallsign(client);
			
			if (inRange && !__underMouse) {
				__underMouse = client;
				
			}
		glPopMatrix();
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);
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
			GLdouble vertices[] = {
					(ap->longitude / 180 - __position.x()) * __zoom,
					(ap->latitude / 90 - __position.y()) * __zoom,
					(pilot->position.longitude / 180 - __position.x()) * __zoom,
					(pilot->position.latitude / 90 - __position.y()) * __zoom
				};
			
			glVertexPointer(2, GL_DOUBLE, 0, vertices);
			glDrawArrays(GL_LINES, 0, 2);
		}
		
		if (!pilot->route.destination.isEmpty())
			ap = __airports[pilot->route.destination]->getData();
		else
			ap = NULL;
		
		if (ap) {
			GLdouble vertices[] = {
					(ap->longitude / 180 - __position.x()) * __zoom,
					(ap->latitude / 90 - __position.y()) * __zoom,
					(pilot->position.longitude / 180 - __position.x()) * __zoom,
					(pilot->position.latitude / 90 - __position.y()) * __zoom
				};
				
			glVertexPointer(2, GL_DOUBLE, 0, vertices);
			glLineStipple(1, 0xF0F0);
			glDrawArrays(GL_LINES, 0, 2);
			glLineStipple(1, 0xFFFF);
			
		}
	} else if (__underMouse->objectType() == AIRPORT) {
		const AirportObject* ap = static_cast< const AirportObject* >(__underMouse);
		if (!ap->getData())
			return;
		
		unsigned linesOut = ap->getOutbounds().size() - ap->countDepartures();
		unsigned linesIn = ap->getInbounds().size() - ap->countArrivals();
		
		GLdouble* vertices = new GLdouble[(linesOut + linesIn) * 4];
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
				drawCallsign(vertices[i], vertices[i+1], p);
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
				drawCallsign(vertices[i], vertices[i+1], p);
			i += 2;
			
			__mapCoordinates(ap->getData()->longitude, ap->getData()->latitude,
							 vertices[i], vertices[i+1]);
			
			i += 2;
		}
		
		glVertexPointer(2, GL_DOUBLE, 0, vertices);
		glColor4f(LINES_COLOR);
		
		glDrawArrays(GL_LINES, 0, linesOut * 2);
		
		glLineStipple(1, 0xF0F0);
		glDrawArrays(GL_LINES, linesOut * 2, linesIn * 2);
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
	
	__circle = new GLdouble[__circleCount * 2 + 2];
	unsigned i = 0;
	
	double x, y;
	for (double angle = 0.0; angle <= (2 * PI); angle += 0.1) {
		x = cos(angle);
		y = sin(angle);
		__circle[i++] = x;
		__circle[i++] = y;
	}
}
